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
}

Oscillator::~Oscillator()
{
    
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
}

void Oscillator::tick(float output[][NUM_STRINGS])
{
    if (!enabled) return;
    
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
        tMBSaw_setFreq(&saw[v], freq);
        tMBPulse_setFreq(&pulse[v], freq);
        
        float sample = 0.0f;
        
        sample += tMBSaw_tick(&saw[v]) * (1.0f - shape);
        sample += tMBPulse_tick(&pulse[v]) * shape;
        sample *= amp*INV_NUM_OSCS;
        
        float f = filterSend->tickNoHooks();
        output[0][v] += sample*f;
        output[1][v] += sample*(1.f-f);
    }
    sampleInBlock++;
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
