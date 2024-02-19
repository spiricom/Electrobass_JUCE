/*
  ==============================================================================

    Output.h
    Created: 17 Jun 2021 10:57:13pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "../Constants.h"
#include "Utilities.h"


//==============================================================================

class Output : public AudioComponent
{
public:
    //==============================================================================
    Output(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~Output();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float input[MAX_NUM_VOICES]);
    std::unique_ptr<SmoothedParameter> master;
    void tickLowpass(float input [MAX_NUM_VOICES]);

private:
    void lowpassTick(float& sample, int v, float cutoff);
    tSVF_LP lowpass[MAX_NUM_VOICES];
    
    
};

