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
    
    void (Filter::*filterTick)(float& sample, int v, float cutoff, float q, float morph);
    void lowpassTick(float& sample, int v, float cutoff, float q, float morph);
    void highpassTick(float& sample, int v, float cutoff, float q, float morph);
    void bandpassTick(float& sample, int v, float cutoff, float q, float morph);
    
    //tVZFilter filters[NUM_STRINGS];
    tSVF lowpass[NUM_STRINGS];
    tSVF highpass[NUM_STRINGS];
    tSVF bandpass[NUM_STRINGS];
    
    std::atomic<float>* afpFilterType;
    FilterType currentFilterType = FilterTypeNil;
};
