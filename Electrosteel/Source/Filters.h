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

class LowpassFilter : public AudioComponent
{
public:
    //==============================================================================
    LowpassFilter(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    ~LowpassFilter();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    float tick(int v, float input);
    
private:
    
    tEfficientSVF lowpass[NUM_VOICES];
};
