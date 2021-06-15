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
Filter::Filter(const String& n, ESAudioProcessor& p,
                             AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cFilterParams, true)
{
    for (int i = 0; i < FilterParamNil; ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tEfficientSVF_init(&svf[i], SVFTypeLowpass, 2000.f, 0.4f, &processor.leaf);
    }
}

Filter::~Filter()
{
    
}

void Filter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

void Filter::frame()
{
    enabled = *afpEnabled > 0;
}

float Filter::tick(int v, float input)
{
    if (!enabled) return input;
    
    float midiCutoff = ref[FilterCutoff][v]->tick();
    float keyFollow = ref[FilterKeyFollow][v]->tick();
    float q = ref[FilterResonance][v]->tick();
    
    float follow = processor.voiceNote[v] - 60.f;
    float cutoff = (midiCutoff * (1.f - keyFollow)) + ((midiCutoff + follow) * keyFollow);
    
    cutoff = LEAF_clip(0.0f, cutoff*32.f, 4095.f);
    tEfficientSVF_setFreqAndQ(&svf[v], cutoff, q);
    
    return tEfficientSVF_tick(&svf[v], input);
}
