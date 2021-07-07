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
AudioComponent(n, p, vts, cOscParams, true)
{
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_init(&saw[i], &processor.leaf);
        tMBPulse_init(&pulse[i], &processor.leaf);
    }
    
    filterSend = std::make_unique<SmoothedParameter>(p, vts, n + " FilterSend", -1);
    
    afpShapeSet = vts.getRawParameterValue(n + " ShapeSet");
}

Oscillator::~Oscillator()
{
    if (tables != nullptr)
    {
        for (int i = 0; i < NUM_STRINGS; ++i)
        {
            tWaveOscS_free(&wave[i]);
        }
        for (int i = 0; i < lastNumWaveTables; ++i)
        {
            tWaveTableS_free(&tables[i]);
        }
        leaf_free(&processor.leaf, (char*)tables);
        tables = nullptr;
    }
}

void Oscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tMBSaw_setSampleRate(&saw[i], sampleRate);
        tMBPulse_setSampleRate(&pulse[i], sampleRate);
    }
}

void Oscillator::frame()
{
    for (int i = 0; i < params.size(); ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            lastValues[i][v] = values[i][v];
            values[i][v] = ref[i][v]->skip(currentBlockSize);
        }
    }
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0;
    currentShapeSet = OscShapeSet(int(*afpShapeSet));
    switch (currentShapeSet) {
        case SawPulseShapeSet:
            shapeTick = &Oscillator::sawPulseTick;
            break;
            
        case UserShapeSet:
            shapeTick = &Oscillator::userTick;
            break;
            
        default:
            shapeTick = &Oscillator::sawPulseTick;
            break;
    }
}

void Oscillator::tick(float output[][NUM_STRINGS])
{
    if (!enabled || loadingTables) return;
    
    float a = sampleInBlock * invBlockSize;

    for (int v = 0; v < NUM_STRINGS; ++v)
    {
        float pitch = values[OscPitch][v]*a + lastValues[OscPitch][v]*(1.f-a);
        float fine = values[OscFine][v]*a + lastValues[OscFine][v]*(1.f-a);
        float shape = values[OscShape][v]*a + lastValues[OscShape][v]*(1.f-a);
        float amp = values[OscAmp][v]*a + lastValues[OscAmp][v]*(1.f-a);
        amp = amp < 0.f ? 0.f : amp;
        
        float note = processor.voiceNote[v];
        float freq = mtof(LEAF_clip(0, note + pitch + fine*0.01f, 127));
        freq = freq < 10.f ? 0.f : freq;
        
        float sample = 0.0f;
        (this->*shapeTick)(sample, v, freq, shape);
    
        sample *= amp*INV_NUM_OSCS;
        
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

void Oscillator::userTick(float& sample, int v, float freq, float shape)
{
    tWaveOscS_setFreq(&wave[v], freq);
    tWaveOscS_setIndex(&wave[v], shape);
    sample += tWaveOscS_tick(&wave[v]);
}

void Oscillator::addWaveTables(File& file)
{
    if (file.isDirectory())
    {
        for (auto wav : file.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav"))
        {
            addWaveTables(wav);
        }
    }
    else
    {
        auto* reader = processor.formatManager.createReaderFor(file);
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource
                                                                      (reader, true));
            jassert(reader->lengthInSamples%2048 == 0);
            
            for (int i = 0; i < reader->lengthInSamples / 2048; ++i)
            {
                waveTables.add(new AudioBuffer<float>(reader->numChannels, 2048));
                reader->read(waveTables.getLast(), 0, waveTables.getLast()->getNumSamples(),
                             i*2048, true, true);
            }
            
            processor.readerSource.reset(newSource.release());
        }
    }
}

void Oscillator::clearWaveTables()
{
    waveTables.clear();
}

void Oscillator::waveTablesChanged()
{
    if (tables != nullptr)
    {
        for (int i = 0; i < NUM_STRINGS; ++i)
        {
            tWaveOscS_free(&wave[i]);
        }
        for (int i = 0; i < lastNumWaveTables; ++i)
        {
            tWaveTableS_free(&tables[i]);
        }
        leaf_free(&processor.leaf, (char*)tables);
        tables = nullptr;
    }
    lastNumWaveTables = waveTables.size();
    
    if (lastNumWaveTables == 0)
    {
        loadingTables = false;
        return;
    }
    
    tables = (tWaveTableS*)leaf_alloc(&processor.leaf, sizeof(tWaveTableS) * lastNumWaveTables);
    for (int i = 0; i < lastNumWaveTables; ++i)
    {
        tWaveTableS_init(&tables[i], waveTables[i]->getWritePointer(0), 2048, 14000.f, &processor.leaf);
    }
    
    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        tWaveOscS_init(&wave[i], tables, waveTables.size(), &processor.leaf);
    }
    loadingTables = false;
}

//==============================================================================

LowFreqOscillator::LowFreqOscillator(const String& n, ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cLowFreqParams, false),
MappingSourceModel(p, n, lfoValues, true, true, true, Colours::lime)
{
    sync = vts.getParameter(n + " Sync");
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tCycle_init(&lfo[i], &processor.leaf);
        lfoValues[i] = (float*)leaf_calloc(&processor.leaf, sizeof(float) * currentBlockSize);
    }
    
    phaseReset = 0.0f;
}

LowFreqOscillator::~LowFreqOscillator()
{
    if (tables != nullptr)
    {
        for (int i = 0; i < NUM_STRINGS; ++i)
        {
            tWaveOscS_free(&wave[i]);
        }
        for (int i = 0; i < lastNumWaveTables; ++i)
        {
            tWaveTableS_free(&tables[i]);
        }
        leaf_free(&processor.leaf, (char*)tables);
        tables = nullptr;
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        leaf_free(&processor.leaf, (char*)lfoValues[i]);
    }
}

void LowFreqOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tCycle_setSampleRate(&lfo[i], sampleRate);
        leaf_free(&processor.leaf, (char*)lfoValues[i]);
    }
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        lfoValues[i] = (float*)leaf_calloc(&processor.leaf, sizeof(float) * currentBlockSize);
    }
    source = lfoValues;
}

void LowFreqOscillator::frame()
{
    for (int i = 0; i < params.size(); ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            lastValues[i][v] = values[i][v];
            values[i][v] = ref[i][v]->skip(currentBlockSize);
        }
    }
    sampleInBlock = 0;
}

void LowFreqOscillator::tick()
{
    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < NUM_STRINGS; v++)
    {
        float rate = values[LowFreqRate][v]*a + lastValues[LowFreqRate][v]*(1.f-a);
        // Even though our oscs can handle negative frequency I think allowing the rate to
        // go negative would be confusing behavior
        rate = rate < 0.f ? 0.f : rate;
        
        tCycle_setFreq(&lfo[v], rate);
        
        lfoValues[v][sampleInBlock] = tCycle_tick(&lfo[v]);
    }
    sampleInBlock++;
}

void LowFreqOscillator::noteOn(int voice, float velocity)
{
    if (sync->getValue() > 0) tCycle_setPhase(&lfo[voice], phaseReset);
}

void LowFreqOscillator::noteOff(int voice, float velocity)
{
    
}

void LowFreqOscillator::addWaveTables(File& file)
{
    if (file.isDirectory())
    {
        for (auto wav : file.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav"))
        {
            addWaveTables(wav);
        }
    }
    else
    {
        auto* reader = processor.formatManager.createReaderFor(file);
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource
                                                                      (reader, true));
            jassert(reader->lengthInSamples%2048 == 0);
            
            for (int i = 0; i < reader->lengthInSamples / 2048; ++i)
            {
                waveTables.add(new AudioBuffer<float>(reader->numChannels, 2048));
                reader->read(waveTables.getLast(), 0, waveTables.getLast()->getNumSamples(),
                             i*2048, true, true);
            }
            
            processor.readerSource.reset(newSource.release());
        }
    }
}

void LowFreqOscillator::clearWaveTables()
{
    waveTables.clear();
}

void LowFreqOscillator::waveTablesChanged()
{
    if (tables != nullptr)
    {
        for (int i = 0; i < NUM_STRINGS; ++i)
        {
            tWaveOscS_free(&wave[i]);
        }
        for (int i = 0; i < lastNumWaveTables; ++i)
        {
            tWaveTableS_free(&tables[i]);
        }
        leaf_free(&processor.leaf, (char*)tables);
        tables = nullptr;
    }
    lastNumWaveTables = waveTables.size();
    
    if (lastNumWaveTables == 0)
    {
        loadingTables = false;
        return;
    }
    
    tables = (tWaveTableS*)leaf_alloc(&processor.leaf, sizeof(tWaveTableS) * lastNumWaveTables);
    for (int i = 0; i < lastNumWaveTables; ++i)
    {
        tWaveTableS_init(&tables[i], waveTables[i]->getWritePointer(0), 2048, 14000.f, &processor.leaf);
    }

    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        tWaveOscS_init(&wave[i], tables, waveTables.size(), &processor.leaf);
    }
    loadingTables = false;
}
