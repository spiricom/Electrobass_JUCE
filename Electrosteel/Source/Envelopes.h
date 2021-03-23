/*
  ==============================================================================

    Envelopes.h
    Created: 17 Mar 2021 4:17:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "Constants.h"
#include "Utilities.h"

class ESAudioProcessor;

class Envelope : public AudioComponent
{
public:
    //==============================================================================
    Envelope(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    ~Envelope();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void tick();
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    float* getValuePointer(int voice);
    
private:
    
    tADSRT envs[NUM_VOICES];
    float value[NUM_VOICES];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
};
