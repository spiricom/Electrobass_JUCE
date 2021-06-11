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
    
    //==============================================================================
    void frame();
    float tick(int v, float input);
    
private:
    
    SmoothedParameter* ref[FilterParamNil][NUM_STRINGS];
    
    tEfficientSVF svf[NUM_STRINGS];
    
    bool enabled;
};
