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

class Envelope : public AudioComponent,
                 public MappingSourceModel
{
public:
    //==============================================================================
    Envelope(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Envelope();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick();
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
private:
    RangedAudioParameter* useVelocity;
    
    tADSRT envs[NUM_STRINGS];
    
    float* sourceValues[MAX_NUM_UNIQUE_SKEWS];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
    float current[EnvelopeParamNil];
    float next[EnvelopeParamNil];
};
