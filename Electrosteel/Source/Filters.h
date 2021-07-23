/*
  ==============================================================================

    Filters.h
    Created: 17 Mar 2021 3:10:49pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "Constants.h"
#include "Utilities.h"

class ESAudioProcessor;

class Filter : public AudioComponent
{
public:
    //==============================================================================
    Filter(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Filter();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float* samples);
    
private:
    
    void (Filter::*filterTick)(float& sample, int v, float cutoff, float q);
    void lowpassTick(float& sample, int v, float cutoff, float q);
    void highpassTick(float& sample, int v, float cutoff, float q);
    void bandpassTick(float& sample, int v, float cutoff, float q);
    
    tEfficientSVF lowpass[NUM_STRINGS];
    tEfficientSVF highpass[NUM_STRINGS];
    tEfficientSVF bandpass[NUM_STRINGS];
    
    std::atomic<float>* afpFilterType;
    FilterType currentFilterType = FilterTypeNil;
};
