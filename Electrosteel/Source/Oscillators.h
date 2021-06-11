/*
  ==============================================================================

    Oscillators.h
    Created: 17 Mar 2021 2:49:31pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "Constants.h"
#include "Utilities.h"

class ESAudioProcessor;

//==============================================================================

class Oscillator : public AudioComponent
{
public:
    //==============================================================================
    Oscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Oscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void frame();
    void tick(int v, float* output);
    
private:
    
    SmoothedParameter* ref[OscParamNil][NUM_STRINGS];
    
    tSawtooth saw[NUM_STRINGS];
    tRosenbergGlottalPulse pulse[NUM_STRINGS];
    
    bool enabled;
    
    std::atomic<float>* afpFilterSend;
    
//    tCycle pwmLFO1;
//    tCycle pwmLFO2;
};

//==============================================================================

class LowFreqOscillator : public AudioComponent
{
public:
    //==============================================================================
    LowFreqOscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~LowFreqOscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void frame();
    void tick();
    
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
    float* getValuePointer();
    
private:
    
    SmoothedParameter* ref[LowFreqParamNil][NUM_STRINGS];
    RangedAudioParameter* sync;
    
    tCycle lfo[NUM_STRINGS];
    float phaseReset;
    float value[NUM_STRINGS];
};
