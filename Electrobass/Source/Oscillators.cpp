/*
  ==============================================================================

    Oscillators.cpp
    Created: 17 Mar 2021 2:49:31pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Oscillators.h"
#include "PluginProcessor.h"

//==============================================================================

Oscillator::Oscillator(const String& n, ESAudioProcessor& p,
                       AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cOscParams, true),
MappingSourceModel(p, n, true, true, Colours::darkorange)
{
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        sourceValues[i] = (float*) leaf_alloc(&p.leaf, sizeof(float) * NUM_STRINGS);
        sources[i] = &sourceValues[i];
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_init(&saw[i], &processor.leaf);
        tMBPulse_init(&pulse[i], &processor.leaf);
        tCycle_init(&sine[i], &processor.leaf);
        tMBTriangle_init(&tri[i], &processor.leaf);
        
        tMBSaw_init(&sawPaired[i], &processor.leaf);
        tMBPulse_init(&pulsePaired[i], &processor.leaf);
        tCycle_init(&sinePaired[i], &processor.leaf);
        tMBTriangle_init(&triPaired[i], &processor.leaf);
    }
    
    filterSend = std::make_unique<SmoothedParameter>(p, vts, n + " FilterSend");
    
    afpShapeSet = vts.getRawParameterValue(n + " ShapeSet");
}

Oscillator::~Oscillator()
{
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        leaf_free(&processor.leaf, (char*)sourceValues[i]);
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_free(&saw[i]);
        tMBPulse_free(&pulse[i]);
        tCycle_free(&sine[i]);
        tMBTriangle_free(&tri[i]);
        
        tMBSaw_free(&sawPaired[i]);
        tMBPulse_free(&pulsePaired[i]);
        tCycle_free(&sinePaired[i]);
        tMBTriangle_free(&triPaired[i]);
        
        if (waveTableFile.exists())
        {
            tWaveOscS_free(&wave[i]);
        }
    }
    DBG("Post exit: " + String(processor.leaf.allocCount) + " " + String(processor.leaf.freeCount));
}

void Oscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_setSampleRate(&saw[i], sampleRate);
        tMBPulse_setSampleRate(&pulse[i], sampleRate);
        tCycle_setSampleRate(&sine[i], sampleRate);
        tMBTriangle_setSampleRate(&tri[i], sampleRate);
        
        tMBSaw_setSampleRate(&sawPaired[i], sampleRate);
        tMBPulse_setSampleRate(&pulsePaired[i], sampleRate);
        tCycle_setSampleRate(&sinePaired[i], sampleRate);
        tMBTriangle_setSampleRate(&triPaired[i], sampleRate);
        
        if (waveTableFile.exists())
        {
            tWaveOscS_setSampleRate(&wave[i], sampleRate);
        }
    }
}

void Oscillator::frame()
{
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0 ||
    processor.sourceMappingCounts[getName()] > 0;
    
    currentShapeSet = OscShapeSet(int(*afpShapeSet));
    switch (currentShapeSet) {
        case SawPulseOscShapeSet:
            shapeTick = &Oscillator::sawSquareTick;
            break;
            
        case SineTriOscShapeSet:
            shapeTick = &Oscillator::sineTriTick;
            break;
            
        case SawOscShapeSet:
            shapeTick = &Oscillator::sawTick;
            break;
            
        case PulseOscShapeSet:
            shapeTick = &Oscillator::pulseTick;
            break;
            
        case SineOscShapeSet:
            shapeTick = &Oscillator::sineTick;
            break;
            
        case TriOscShapeSet:
            shapeTick = &Oscillator::triTick;
            break;
            
        case UserOscShapeSet:
            shapeTick = &Oscillator::userTick;
            break;
            
        default:
            shapeTick = &Oscillator::sawSquareTick;
            break;
    }
}

void Oscillator::tick(float output[][NUM_STRINGS])
{
    if (loadingTables || !enabled) return;

    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        if (!processor.voiceIsSounding[v]) continue;
        
        float pitch = quickParams[OscPitch][v]->tickNoSmoothing();
        float fine = quickParams[OscFine][v]->tickNoSmoothing();
        float freq = quickParams[OscFreq][v]->tickNoSmoothing();
        float shape = quickParams[OscShape][v]->tickNoSmoothing();
        float amp = quickParams[OscAmp][v]->tickNoSmoothing();
        
        amp = amp < 0.f ? 0.f : amp;
        
        float note = processor.voiceNote[v];
        float finalFreq = mtof(LEAF_clip(0, note + pitch + fine*0.01f, 127)) + freq;
        //freq = freq < 10.f ? 0.f : freq
        
        float sample = 0.0f;
        
        shape = LEAF_clip(0.f, shape, 1.f);
        (this->*shapeTick)(sample, v, finalFreq, shape);
    
        sample *= amp;
        
        float normSample = (sample + 1.f) * 0.5f;
        sourceValues[0][v] = normSample;
        for (int i = 1; i < processor.numInvParameterSkews; ++i)
        {
            float invSkew = processor.quickInvParameterSkews[i];
            sourceValues[i][v] = powf(normSample, invSkew);
        }
        
        sample *= INV_NUM_OSCS;
        
        float f = filterSend->tickNoHooks();
        
        output[0][v] += sample*f * *afpEnabled;
        output[1][v] += sample*(1.f-f) * *afpEnabled ;
    }
    
    sampleInBlock++;
}

void Oscillator::sawSquareTick(float& sample, int v, float freq, float shape)
{
    tMBSaw_setFreq(&sawPaired[v], freq);
    tMBPulse_setFreq(&pulsePaired[v], freq);
    sample += tMBSaw_tick(&sawPaired[v]) * (1.0f - shape) * 2.f;
    sample += tMBPulse_tick(&pulsePaired[v]) * shape * 2.f;
}

void Oscillator::sineTriTick(float& sample, int v, float freq, float shape)
{
    tCycle_setFreq(&sinePaired[v], freq);
    tMBTriangle_setFreq(&triPaired[v], freq);
    sample += tCycle_tick(&sinePaired[v]) * (1.0f - shape);
    sample += tMBTriangle_tick(&triPaired[v]) * shape * 2.f;;
}

void Oscillator::sawTick(float& sample, int v, float freq, float shape)
{
    tMBSaw_setFreq(&saw[v], freq);
    sample += tMBSaw_tick(&saw[v]) * 2.f;;
}

void Oscillator::pulseTick(float& sample, int v, float freq, float shape)
{
    tMBPulse_setFreq(&pulse[v], freq);
    tMBPulse_setWidth(&pulse[v], shape);
    sample += tMBPulse_tick(&pulse[v]) * 2.f;;
}

void Oscillator::sineTick(float& sample, int v, float freq, float shape)
{
    tCycle_setFreq(&sine[v], freq);
    sample += tCycle_tick(&sine[v]);
}

void Oscillator::triTick(float& sample, int v, float freq, float shape)
{
    tMBTriangle_setFreq(&tri[v], freq);
    tMBTriangle_setWidth(&tri[v], shape);
    sample += tMBTriangle_tick(&tri[v]) * 2.f;;
}

void Oscillator::userTick(float& sample, int v, float freq, float shape)
{
    tWaveOscS_setFreq(&wave[v], freq);
    tWaveOscS_setIndex(&wave[v], shape);
    sample += tWaveOscS_tick(&wave[v]);
}

void Oscillator::setWaveTables(File file)
{
    loadingTables = true;
    
    File loaded = processor.loadWaveTables(file.getFullPathName(), file);
    
    if (loaded.exists() && loaded != waveTableFile)
    {
        if (waveTableFile.exists())
        {
            for (int i = 0; i < NUM_STRINGS; ++i)
            {
                tWaveOscS_free(&wave[i]);
            }
        }
        waveTableFile = loaded;
        
        for (int i = 0; i < NUM_STRINGS; ++i)
        {
            Array<tWaveTableS>& tables = processor.waveTables.getReference(waveTableFile.getFullPathName());
            tWaveOscS_init(&wave[i], tables.getRawDataPointer(), tables.size(), &processor.leaf);
        }
    }
    
    loadingTables = false;
}

//==============================================================================

LowFreqOscillator::LowFreqOscillator(const String& n, ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cLowFreqParams, false),
MappingSourceModel(p, n, true, true, Colours::chartreuse)
{
    for (int i = 0; i < p.numInvParameterSkews; ++i)
    {
        sourceValues[i] = (float*) leaf_alloc(&p.leaf, sizeof(float) * NUM_STRINGS);
        sources[i] = &sourceValues[i];
    }
    
    sync = vts.getParameter(n + " Sync");
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_init(&saw[i], &processor.leaf);
        tMBPulse_init(&pulse[i], &processor.leaf);
        tCycle_init(&sine[i], &processor.leaf);
        tMBTriangle_init(&tri[i], &processor.leaf);
        
        tMBSaw_init(&sawPaired[i], &processor.leaf);
        tMBPulse_init(&pulsePaired[i], &processor.leaf);
        tCycle_init(&sinePaired[i], &processor.leaf);
        tMBTriangle_init(&triPaired[i], &processor.leaf);
    }
    
    phaseReset = 0.0f;
    
    afpShapeSet = vts.getRawParameterValue(n + " ShapeSet");
}

LowFreqOscillator::~LowFreqOscillator()
{
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        leaf_free(&processor.leaf, (char*)sourceValues[i]);
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_free(&saw[i]);
        tMBPulse_free(&pulse[i]);
        tCycle_free(&sine[i]);
        tMBTriangle_free(&tri[i]);
        
        tMBSaw_free(&sawPaired[i]);
        tMBPulse_free(&pulsePaired[i]);
        tCycle_free(&sinePaired[i]);
        tMBTriangle_free(&triPaired[i]);
    }
}

void LowFreqOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_setSampleRate(&saw[i], sampleRate);
        tMBPulse_setSampleRate(&pulse[i], sampleRate);
        tCycle_setSampleRate(&sine[i], sampleRate);
        tMBTriangle_setSampleRate(&tri[i], sampleRate);
        
        tMBSaw_setSampleRate(&sawPaired[i], sampleRate);
        tMBPulse_setSampleRate(&pulsePaired[i], sampleRate);
        tCycle_setSampleRate(&sinePaired[i], sampleRate);
        tMBTriangle_setSampleRate(&triPaired[i], sampleRate);
    }
}

void LowFreqOscillator::frame()
{
    sampleInBlock = 0;
    // only enabled if it's actually being used as a source
    enabled = processor.sourceMappingCounts[getName()] > 0;
    currentShapeSet = LFOShapeSet(int(*afpShapeSet));
    switch (currentShapeSet) {
        case SineTriLFOShapeSet:
            shapeTick = &LowFreqOscillator::sineTriTick;
            break;
            
        case SawPulseLFOShapeSet:
            shapeTick = &LowFreqOscillator::sawSquareTick;
            break;
            
        case SineLFOShapeSet:
            shapeTick = &LowFreqOscillator::sineTick;
            break;
            
        case TriLFOShapeSet:
            shapeTick = &LowFreqOscillator::triTick;
            break;
            
        case SawLFOShapeSet:
            shapeTick = &LowFreqOscillator::sawTick;
            break;
            
        case PulseLFOShapeSet:
            shapeTick = &LowFreqOscillator::pulseTick;
            break;
            
        default:
            shapeTick = &LowFreqOscillator::sineTriTick;
            break;
    }
}

void LowFreqOscillator::tick()
{
    if (!enabled) return;
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; v++)
    {
        float rate = quickParams[LowFreqRate][v]->tickNoSmoothing();
        float shape = quickParams[LowFreqShape][v]->tickNoSmoothing();
        // Even though our oscs can handle negative frequency I think allowing the rate to
        // go negative would be confusing behavior
        rate = rate < 0.f ? 0.f : rate;
        shape = LEAF_clip(0.f, shape, 1.f);
        
        float sample = 0;
        (this->*shapeTick)(sample, v, rate, shape);
        
        float normSample = (sample + 1.f) * 0.5f;
        sourceValues[0][v] = normSample;
        for (int i = 1; i < processor.numInvParameterSkews; ++i)
        {
            float invSkew = processor.quickInvParameterSkews[i];
            sourceValues[i][v] = powf(normSample, invSkew);
        }
    }
    sampleInBlock++;
}

void LowFreqOscillator::sawSquareTick(float& sample, int v, float rate, float shape)
{
    tMBSaw_setFreq(&sawPaired[v], rate);
    tMBPulse_setFreq(&pulsePaired[v], rate);
    sample += tMBSaw_tick(&sawPaired[v]) * (1.0f - shape) * 2.f;;
    sample += tMBPulse_tick(&pulsePaired[v]) * shape * 2.f;;
}

void LowFreqOscillator::sineTriTick(float& sample, int v, float rate, float shape)
{
    tCycle_setFreq(&sinePaired[v], rate);
    tMBTriangle_setFreq(&triPaired[v], rate);
    sample += tCycle_tick(&sinePaired[v]) * (1.0f - shape);
    sample += tMBTriangle_tick(&triPaired[v]) * shape * 2.f;;
}

void LowFreqOscillator::sineTick(float& sample, int v, float freq, float shape)
{
    tCycle_setFreq(&sine[v], freq);
    sample += tCycle_tick(&sine[v]);
}

void LowFreqOscillator::triTick(float& sample, int v, float freq, float shape)
{
    tMBTriangle_setFreq(&tri[v], freq);
    tMBTriangle_setWidth(&tri[v], shape);
    sample += tMBTriangle_tick(&tri[v]) * 2.f;;
}

void LowFreqOscillator::sawTick(float& sample, int v, float freq, float shape)
{
    tMBSaw_setFreq(&saw[v], freq);
    sample += tMBSaw_tick(&saw[v]) * 2.f;;
}

void LowFreqOscillator::pulseTick(float& sample, int v, float freq, float shape)
{
    tMBPulse_setFreq(&pulse[v], freq);
    tMBPulse_setWidth(&pulse[v], shape);
    sample += tMBPulse_tick(&pulse[v]) * 2.f;;
}

void LowFreqOscillator::noteOn(int voice, float velocity)
{
    if (sync->getValue() > 0)
    {
        tMBSaw_setPhase(&saw[voice], phaseReset);
        tMBPulse_setPhase(&pulse[voice], phaseReset);
        tCycle_setPhase(&sine[voice], phaseReset);
        tMBTriangle_setPhase(&tri[voice], phaseReset);
        
        tMBSaw_setPhase(&sawPaired[voice], phaseReset);
        tMBPulse_setPhase(&pulsePaired[voice], phaseReset);
        tCycle_setPhase(&sinePaired[voice], phaseReset);
        tMBTriangle_setPhase(&triPaired[voice], phaseReset);
    }
}


//==============================================================================

NoiseGenerator::NoiseGenerator(const String& n, ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cNoiseParams, true),
MappingSourceModel(p, n, true, true, Colours::darkorange)
{
    for (int i = 0; i < p.numInvParameterSkews; ++i)
    {
        sourceValues[i] = (float*) leaf_alloc(&p.leaf, sizeof(float) * NUM_STRINGS);
        sources[i] = &sourceValues[i];
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tNoise_init(&noise[i], WhiteNoise, &processor.leaf);
        tSVF_init(&bandpass[i], SVFTypeBandpass, 2000.f, 0.7f, &processor.leaf);
    }
    
    filterSend = std::make_unique<SmoothedParameter>(p, vts, n + " FilterSend");

//    afpShapeSet = vts.getRawParameterValue(n + " ShapeSet");
}

NoiseGenerator::~NoiseGenerator()
{
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        leaf_free(&processor.leaf, (char*)sourceValues[i]);
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tNoise_free(&noise[i]);
        tSVF_free(&bandpass[i]);
    }
}

void NoiseGenerator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tSVF_setSampleRate(&bandpass[i], sampleRate);
    }
}

void NoiseGenerator::frame()
{
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0 ||
    processor.sourceMappingCounts[getName()] > 0;
    
//    currentShapeSet = LFOShapeSet(int(*afpShapeSet));
//    switch (currentShapeSet) {
//        case SineTriLFOShapeSet:
//            shapeTick = &LowFreqOscillator::sineTriTick;
//            break;
//
//        case SawPulseLFOShapeSet:
//            shapeTick = &LowFreqOscillator::sawSquareTick;
//            break;
//
//        case SineLFOShapeSet:
//            shapeTick = &LowFreqOscillator::sineTick;
//            break;
//
//        case TriLFOShapeSet:
//            shapeTick = &LowFreqOscillator::triTick;
//            break;
//
//        case SawLFOShapeSet:
//            shapeTick = &LowFreqOscillator::sawTick;
//            break;
//
//        case PulseLFOShapeSet:
//            shapeTick = &LowFreqOscillator::pulseTick;
//            break;
//
//        default:
//            shapeTick = &LowFreqOscillator::sineTriTick;
//            break;
//    }
}

void NoiseGenerator::tick(float output[][NUM_STRINGS])
{
    if (!enabled) return;
    //    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; v++)
    {
        float color = quickParams[NoiseColor][v]->tickNoSmoothing();
        float amp = quickParams[NoiseAmp][v]->tickNoSmoothing();
        color = color < 0.f ? 0.f : color;
        color = mtof(color*100.f + 24.f);
        amp = amp < 0.f ? 0.f : amp;
    
        tSVF_setFreq(&bandpass[v], color);
        float sample = tSVF_tick(&bandpass[v], tNoise_tick(&noise[v])) * amp;
        
        float normSample = (sample + 1.f) * 0.5f;
        sourceValues[0][v] = normSample;
        for (int i = 1; i < processor.numInvParameterSkews; ++i)
        {
            float invSkew = processor.quickInvParameterSkews[i];
            sourceValues[i][v] = powf(normSample, invSkew);
        }
        
        float f = filterSend->tickNoHooks();
        
        output[0][v] += sample*f * *afpEnabled;
        output[1][v] += sample*(1.f-f) * *afpEnabled ;
    }
    sampleInBlock++;
}
