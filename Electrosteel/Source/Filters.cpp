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
        tEfficientSVF_init(&lowpass[i], SVFTypeLowpass, 2000.f, 0.7f, &processor.leaf);
        tEfficientSVF_init(&highpass[i], SVFTypeHighpass, 2000.f, 0.7f, &processor.leaf);
        tEfficientSVF_init(&bandpass[i], SVFTypeBandpass, 2000.f, 0.7f, &processor.leaf);
    }
    
    afpFilterType = vts.getRawParameterValue(n + " Type");
}

Filter::~Filter()
{
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tEfficientSVF_free(&lowpass[i]);
        tEfficientSVF_free(&highpass[i]);
        tEfficientSVF_free(&bandpass[i]);
    }
}

void Filter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

void Filter::frame()
{
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0;
    
    currentFilterType = FilterType(int(*afpFilterType));
    switch (currentFilterType) {
        case LowpassFilter:
            filterTick = &Filter::lowpassTick;
            break;
            
        case HighpassFilter:
            filterTick = &Filter::highpassTick;
            break;
            
        case BandpassFilter:
            filterTick = &Filter::bandpassTick;
            break;
            
        default:
            filterTick = &Filter::lowpassTick;
            break;
    }
}

void Filter::tick(float* samples)
{
    if (!enabled) return;
    
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float midiCutoff = quickParams[FilterCutoff][v]->tick();
        float keyFollow = quickParams[FilterKeyFollow][v]->tick();
        float q = quickParams[FilterResonance][v]->tick();
        
        LEAF_clip(0.f, keyFollow, 1.f);
        
        float follow = processor.voiceNote[v] - 60.f;
        float cutoff = (midiCutoff * (1.f - keyFollow)) + ((midiCutoff + follow) * keyFollow);
        
        cutoff = LEAF_clip(0.0f, cutoff*32.f, 4095.f);
        q = q < 0.f ? 0.f : q;
        
        (this->*filterTick)(samples[v], v, cutoff, q);
    }
    
    sampleInBlock++;
}

void Filter::lowpassTick(float& sample, int v, float cutoff, float q)
{
    tEfficientSVF_setFreqAndQ(&lowpass[v], cutoff, q);
    sample = tEfficientSVF_tick(&lowpass[v], sample);
}

void Filter::highpassTick(float& sample, int v, float cutoff, float q)
{
    tEfficientSVF_setFreqAndQ(&highpass[v], cutoff, q);
    sample = tEfficientSVF_tick(&highpass[v], sample);
}

void Filter::bandpassTick(float& sample, int v, float cutoff, float q)
{
    tEfficientSVF_setFreqAndQ(&bandpass[v], cutoff, q);
    sample = tEfficientSVF_tick(&bandpass[v], sample);
}

