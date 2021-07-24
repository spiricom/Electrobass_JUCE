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
    for (int i = 0; i < p.numInvParameterSkews; ++i)
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
    }
    
    filterSend = std::make_unique<SmoothedParameter>(p, vts, n + " FilterSend", -1);
    
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
            shapeTick = &Oscillator::sawPulseTick;
            break;
            
        case SineTriOscShapeSet:
            shapeTick = &Oscillator::sineTriTick;
            break;
            
        case UserOscShapeSet:
            shapeTick = &Oscillator::userTick;
            break;
            
        default:
            shapeTick = &Oscillator::sawPulseTick;
            break;
    }
}

void Oscillator::tick(float output[][NUM_STRINGS])
{
    if (loadingTables || !enabled) return;

    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float pitch = quickParams[OscPitch][v]->tick();
        float fine = quickParams[OscFine][v]->tick();
        float shape = quickParams[OscShape][v]->tick();
        float amp = quickParams[OscAmp][v]->tick();
        amp = amp < 0.f ? 0.f : amp;
        
        float note = processor.voiceNote[v];
        float freq = mtof(LEAF_clip(0, note + pitch + fine*0.01f, 127));
        freq = freq < 10.f ? 0.f : freq;
        
        float sample = 0.0f;
        
        shape = LEAF_clip(0.f, shape, 1.f);
        (this->*shapeTick)(sample, v, freq, shape);
    
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
        
        output[0][v] += sample*f;
        output[1][v] += sample*(1.f-f);
    }
    
    sampleInBlock++;
}

void Oscillator::sawPulseTick(float& sample, int v, float freq, float shape)
{
    tMBSaw_setFreq(&saw[v], freq);
    tMBPulse_setFreq(&pulse[v], freq);
    sample += tMBSaw_tick(&saw[v]) * (1.0f - shape);
    sample += tMBPulse_tick(&pulse[v]) * shape;
}

void Oscillator::sineTriTick(float& sample, int v, float freq, float shape)
{
    tCycle_setFreq(&sine[v], freq);
    tMBTriangle_setFreq(&tri[v], freq);
    sample += tCycle_tick(&sine[v]) * (1.0f - shape);
    sample += tMBTriangle_tick(&tri[v]) * shape;
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
        tSawtooth_init(&saw[i], &processor.leaf);
        tSquare_init(&square[i], &processor.leaf);
        tCycle_init(&sine[i], &processor.leaf);
        tTriangle_init(&tri[i], &processor.leaf);
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
        tSawtooth_free(&saw[i]);
        tSquare_free(&square[i]);
        tCycle_free(&sine[i]);
        tTriangle_free(&tri[i]);
    }
}

void LowFreqOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tSawtooth_setSampleRate(&saw[i], sampleRate);
        tSquare_setSampleRate(&square[i], sampleRate);
        tCycle_setSampleRate(&sine[i], sampleRate);
        tTriangle_setSampleRate(&tri[i], sampleRate);
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
        float rate = quickParams[LowFreqRate][v]->tick();
        float shape = quickParams[LowFreqShape][v]->tick();
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
    tSawtooth_setFreq(&saw[v], rate);
    tSquare_setFreq(&square[v], rate);
    sample += tSawtooth_tick(&saw[v]) * (1.0f - shape);
    sample += tSquare_tick(&square[v]) * shape;
}

void LowFreqOscillator::sineTriTick(float& sample, int v, float rate, float shape)
{
    tCycle_setFreq(&sine[v], rate);
    tTriangle_setFreq(&tri[v], rate);
    sample += tCycle_tick(&sine[v]) * (1.0f - shape);
    sample += tTriangle_tick(&tri[v]) * shape;
}

void LowFreqOscillator::noteOn(int voice, float velocity)
{
    if (sync->getValue() > 0)
    {
        tSawtooth_setPhase(&saw[voice], phaseReset);
        tSquare_setPhase(&square[voice], phaseReset);
        tCycle_setPhase(&sine[voice], phaseReset);
        tTriangle_setPhase(&tri[voice], phaseReset);
    }
}

void LowFreqOscillator::noteOff(int voice, float velocity)
{
    
}
