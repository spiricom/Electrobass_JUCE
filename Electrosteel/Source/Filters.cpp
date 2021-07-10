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
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tEfficientSVF_init(&svf[i], SVFTypeLowpass, 2000.f, 0.7f, &processor.leaf);
    }
}

Filter::~Filter()
{
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tEfficientSVF_free(&svf[i]);
    }
}

void Filter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

void Filter::frame()
{
    for (int i = 0; i < params.size(); ++i)
    {
        for (int v = 0; v < processor.numVoicesActive; ++v)
        {
            lastValues[i][v] = values[i][v];
            values[i][v] = ref[i][v]->skip(currentBlockSize);
        }
    }
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0;
}

void Filter::tick(float* samples)
{
    if (!enabled) return;
    
    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float midiCutoff = values[FilterCutoff][v]*a + lastValues[FilterCutoff][v]*(1.f-a);
        float keyFollow = values[FilterKeyFollow][v]*a + lastValues[FilterKeyFollow][v]*(1.f-a);
        float q = values[FilterResonance][v]*a + lastValues[FilterResonance][v]*(1.f-a);
        
        LEAF_clip(0.f, keyFollow, 1.f);
        
        float follow = processor.voiceNote[v] - 60.f;
        float cutoff = (midiCutoff * (1.f - keyFollow)) + ((midiCutoff + follow) * keyFollow);
        
        cutoff = LEAF_clip(0.0f, cutoff*32.f, 4095.f);
        q = q < 0.f ? 0.f : q;
        
        tEfficientSVF_setFreqAndQ(&svf[v], cutoff, q);
        samples[v] = tEfficientSVF_tick(&svf[v], samples[v]);
    }
    
    sampleInBlock++;
}
