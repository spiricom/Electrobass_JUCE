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

// This class might not be needed but keeping for now in case we want
// oscillators to share some behavior down the line
class Oscillator : public AudioComponent
{
public:
    //==============================================================================
    Oscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    virtual ~Oscillator() = default;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
//    virtual void frame() = 0;
    virtual float tick(int v) = 0;
    
private:

};

//==============================================================================
//==============================================================================

class SawPulseOscillator : public Oscillator
{
public:
    //==============================================================================
    SawPulseOscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    ~SawPulseOscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
//    void frame() override;
    float tick(int v) override;
    
private:
    
    tSawtooth saw[NUM_VOICES][NUM_OSC_PER_VOICE];
    tRosenbergGlottalPulse pulse[NUM_VOICES][NUM_OSC_PER_VOICE];
    
    float detune[NUM_VOICES][NUM_OSC_PER_VOICE];
//    tCycle pwmLFO1;
//    tCycle pwmLFO2;
};
