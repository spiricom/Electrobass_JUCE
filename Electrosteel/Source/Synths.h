/*
  ==============================================================================

    Synths.h
    Created: 15 Mar 2021 11:29:46am
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Utilities.h"

class ESAudioProcessor;

class SharedSynthResources
{
public:
    //==============================================================================
    SharedSynthResources(ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~SharedSynthResources();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);

    //==============================================================================
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;

    tADSRT polyEnvs[NUM_VOICES];
    tEfficientSVF synthLP[NUM_VOICES];
    uint16_t filtFreqs[NUM_VOICES];
    tADSRT polyFiltEnvs[NUM_VOICES];

    float detune[NUM_VOICES * NUM_OSC_PER_VOICE];

    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;

    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;

    //==============================================================================
    void calcVoiceFreq(int voice);
};

//==============================================================================
//==============================================================================

class SubtractiveSynth
{
public:
    //==============================================================================
    SubtractiveSynth(ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~SubtractiveSynth();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void frame();
    float tick();
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
private:
    
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
    Array<SmoothedParameter> params;
};

//==============================================================================
//==============================================================================

class WavetableSynth
{
public:
    //==============================================================================
    WavetableSynth(ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~WavetableSynth();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void frame();
    float tick();
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
private:
    
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
    Array<SmoothedParameter> params;
    
    AudioBuffer<float> wavetable;
    
    tWaveSynth waveSynth;
};
