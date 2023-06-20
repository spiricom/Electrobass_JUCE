/*
  ==============================================================================

    Envelopes.h
    Created: 17 Mar 2021 4:17:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "../Constants.h"
#include "Utilities.h"

class ElectroAudioProcessor;

class Envelope : public AudioComponent,
                 public MappingSourceModel
{
public:
    //==============================================================================
    Envelope(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~Envelope();
    
    //==============================================================================
    void setParams() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick();
    void loadAll(int);
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    void setIsAmpEnv(bool isAmpEnv) {this->isAmpEnv = isAmpEnv;}
private:
    RangedAudioParameter* useVelocity;
    bool isAmpEnv = false;
    tADSRT envs[MAX_NUM_VOICES];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
    float current[EnvelopeParamNil];
    float next[EnvelopeParamNil];
};
