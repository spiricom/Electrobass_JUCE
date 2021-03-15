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
    
    Array<std::atomic<float>*> pitchBendValues;
    Array<std::atomic<float>*> ccValues;
    
    LEAF leaf;
    char dummy_memory[1];
    tSimplePoly voice[NUM_VOICES];
    
    float freq[NUM_VOICES];
    float centsDeviation[12];
    int currentTuning;
    int keyCenter;
    
    float detune[NUM_VOICES * NUM_OSC_PER_VOICE];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
    //==============================================================================
    void calcVoiceFreq(int voice);
};

class SubtractiveSynth
{
public:
    //==============================================================================
    SubtractiveSynth(ESAudioProcessor&, AudioProcessorValueTreeState&, SharedSynthResources&);
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
    SharedSynthResources& shared;
    
    Array<std::atomic<float>*> params;
    
    tSawtooth osc[NUM_VOICES * NUM_OSC_PER_VOICE];
    tRosenbergGlottalPulse glottal[NUM_VOICES * NUM_OSC_PER_VOICE];
    
    tEfficientSVF synthLP[NUM_VOICES];
    uint16_t filtFreqs[NUM_VOICES];
    tADSRT polyEnvs[NUM_VOICES];
    tADSRT polyFiltEnvs[NUM_VOICES];
    tCycle pwmLFO1;
    tCycle pwmLFO2;
};
