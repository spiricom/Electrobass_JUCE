/*
 ==============================================================================
 
 Filters.cpp
 Created: 17 Mar 2021 3:10:49pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#include "Filters.h"
#include "PluginProcessor.h"

//==============================================================================
LowpassFilter::LowpassFilter(const String& n, ESAudioProcessor& p,
                             AudioProcessorValueTreeState& vts, StringArray s) :
AudioComponent(n, p, vts, s)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tEfficientSVF_init(&lowpass[i], SVFTypeLowpass, 2000.f, 0.4f, &processor.leaf);
    }
}

LowpassFilter::~LowpassFilter()
{
    
}

void LowpassFilter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
}

float LowpassFilter::tick(int v, float input)
{
    float follow = processor.voiceNote[v] - 60.f;
    
    float midiCutoff = LEAF_frequencyToMidi(params[v][LowpassCutoff]->tick());
    float keyFollow = params[v][LowpassKeyFollow]->tick();
    float q = params[v][LowpassResonance]->tick();
    
    float adjustedMidiCutoff = (midiCutoff * (1.f - keyFollow)) + ((midiCutoff + follow) * keyFollow);
    float cutoff = LEAF_midiToFrequency(adjustedMidiCutoff);
    
    cutoff = LEAF_clip(0.0f, cutoff, 4095.f);
    tEfficientSVF_setFreq(&lowpass[v], cutoff);
    tEfficientSVF_setQ(&lowpass[v], q);
    
    return tEfficientSVF_tick(&lowpass[v], input);
}
