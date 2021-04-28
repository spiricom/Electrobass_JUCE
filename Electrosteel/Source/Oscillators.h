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

class SawPulseOscillator : public AudioComponent
{
public:
    //==============================================================================
    SawPulseOscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    ~SawPulseOscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
//    void frame() override;
    float tick(int v);
    
private:
    
    SmoothedParameter* ref[SawPulseParamNil][NUM_VOICES];
    
    tSawtooth saw[NUM_VOICES][NUM_OSC_PER_VOICE];
    tRosenbergGlottalPulse pulse[NUM_VOICES][NUM_OSC_PER_VOICE];
    
    float detune[NUM_VOICES][NUM_OSC_PER_VOICE];
//    tCycle pwmLFO1;
//    tCycle pwmLFO2;
};
