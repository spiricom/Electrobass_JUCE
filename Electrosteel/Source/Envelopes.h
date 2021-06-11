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
    Envelope(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Envelope();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void frame();
    void tick();
    
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);

    float* getValuePointer();
    
private:
    
    SmoothedParameter* ref[EnvelopeParamNil][NUM_STRINGS];
    RangedAudioParameter* useVelocity;

    tADSRT envs[NUM_STRINGS];
    float value[NUM_STRINGS];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
    int framePosition;
    
    float current[EnvelopeParamNil];
    float next[EnvelopeParamNil];
};
