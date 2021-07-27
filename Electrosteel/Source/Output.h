/*
  ==============================================================================

    Output.h
    Created: 17 Jun 2021 10:57:13pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "Constants.h"
#include "Utilities.h"
#define MASTER_OVERSAMPLE 2
//==============================================================================

class Output : public AudioComponent
{
public:
    //==============================================================================
    Output(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Output();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float input[NUM_STRINGS], float output[2], int numChannels);
    
private:
    
    std::unique_ptr<SmoothedParameter> master;
    tOversampler os[2];
    float oversamplerArray[MASTER_OVERSAMPLE];
};

