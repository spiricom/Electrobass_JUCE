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

class Oscillator : public AudioComponent,
                   public MappingSourceModel
{
public:
    //==============================================================================
    Oscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Oscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float output[][NUM_STRINGS]);
    
    //==============================================================================
    void setWaveTables(File file);
    File& getWaveTableFile() { return waveTableFile; }
    
    OscShapeSet getCurrentShapeSet() { return currentShapeSet; }
    
private:
    
    void (Oscillator::*shapeTick)(float& sample, int v, float freq, float shape);
    void sawSquareTick(float& sample, int v, float freq, float shape);
    void sineTriTick(float& sample, int v, float freq, float shape);
    void sawTick(float& sample, int v, float freq, float shape);
    void pulseTick(float& sample, int v, float freq, float shape);
    void sineTick(float& sample, int v, float freq, float shape);
    void triTick(float& sample, int v, float freq, float shape);
    void userTick(float& sample, int v, float freq, float shape);
    
    tMBSaw saw[NUM_STRINGS];
    tMBPulse pulse[NUM_STRINGS];
    tCycle sine[NUM_STRINGS];
    tMBTriangle tri[NUM_STRINGS];
    
    // Using seperate objects for pairs to easily maintain phase relation
    tMBSaw sawPaired[NUM_STRINGS];
    tMBPulse pulsePaired[NUM_STRINGS];
    tCycle sinePaired[NUM_STRINGS];
    tMBTriangle triPaired[NUM_STRINGS];
    
    tWaveOscS wave[NUM_STRINGS];
    
    float* sourceValues[MAX_NUM_UNIQUE_SKEWS];

    std::unique_ptr<SmoothedParameter> filterSend;
    
    std::atomic<float>* afpShapeSet;
    OscShapeSet currentShapeSet = OscShapeSetNil;
    
    float outSamples[2][NUM_STRINGS];
    
    File waveTableFile;
    bool loadingTables = false;
};

//==============================================================================

class LowFreqOscillator : public AudioComponent,
                          public MappingSourceModel
{
public:
    //==============================================================================
    LowFreqOscillator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~LowFreqOscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick();
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
    //==============================================================================
    LFOShapeSet getCurrentShapeSet() { return currentShapeSet; }
    
private:
    
    void (LowFreqOscillator::*shapeTick)(float& sample, int v, float freq, float shape);
    void sawSquareTick(float& sample, int v, float freq, float shape);
    void sineTriTick(float& sample, int v, float freq, float shape);
    void userTick(float& sample, int v, float freq, float shape);
    void sawTick(float& sample, int v, float freq, float shape);
    void pulseTick(float& sample, int v, float freq, float shape);
    void sineTick(float& sample, int v, float freq, float shape);
    void triTick(float& sample, int v, float freq, float shape);
    
    RangedAudioParameter* sync;
    
    tMBSaw saw[NUM_STRINGS];
    tMBPulse pulse[NUM_STRINGS];
    tCycle sine[NUM_STRINGS];
    tMBTriangle tri[NUM_STRINGS];
    
    // Using seperate objects for pairs to easily maintain phase relation
    tMBSaw sawPaired[NUM_STRINGS];
    tMBPulse pulsePaired[NUM_STRINGS];
    tCycle sinePaired[NUM_STRINGS];
    tMBTriangle triPaired[NUM_STRINGS];
    
    tWaveOscS wave[NUM_STRINGS];
    
    float* sourceValues[MAX_NUM_UNIQUE_SKEWS];
    float phaseReset;
    
    std::atomic<float>* afpShapeSet;
    LFOShapeSet currentShapeSet = LFOShapeSetNil;
};

//==============================================================================

class NoiseGenerator : public AudioComponent,
public MappingSourceModel
{
public:
    //==============================================================================
    NoiseGenerator(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~NoiseGenerator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float output[][NUM_STRINGS]);
    
private:
    
    tNoise noise[NUM_STRINGS];
    tSVF bandpass[NUM_STRINGS];
    
    std::unique_ptr<SmoothedParameter> filterSend;
    
    float* sourceValues[MAX_NUM_UNIQUE_SKEWS];
};
