/*
  ==============================================================================

    Oscillators.h
    Created: 17 Mar 2021 2:49:31pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "../Constants.h"
#include "Utilities.h"

class ElectroAudioProcessor;

//==============================================================================

class Oscillator : public AudioComponent,
                   public MappingSourceModel
{
public:
    //==============================================================================
    Oscillator(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~Oscillator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float output[][MAX_NUM_VOICES]);
    void loadAll(int);
    //==============================================================================
    void setWaveTables(File file);
    File& getWaveTableFile() { return waveTableFile; }
    void setMtoF(float mn);
    OscShapeSet getCurrentShapeSet() { return currentShapeSet; }
    void setSyncSource(Oscillator* osc) {syncSource = osc;}
    float syncOut[MAX_NUM_VOICES];
    bool getEnabled()
    {
        if (afpEnabled == nullptr || *afpEnabled > 0)
        {
            return true;
        } else
        {
            return false;
        }
    }
    
    void setShape(int v, float shape);
private:
    void (Oscillator::*shapeTick)(float& sample, int v, float freq, float shape);
    void sawSquareTick(float& sample, int v, float freq, float shape);
    void sineTriTick(float& sample, int v, float freq, float shape);
    void sawTick(float& sample, int v, float freq, float shape);
    void pulseTick(float& sample, int v, float freq, float shape);
    void sineTick(float& sample, int v, float freq, float shape);
    void triTick(float& sample, int v, float freq, float shape);
    void userTick(float& sample, int v, float freq, float shape);
    
    tMBSaw saw[MAX_NUM_VOICES];
    tMBPulse pulse[MAX_NUM_VOICES];
    tCycle sine[MAX_NUM_VOICES];
    tMBTriangle tri[MAX_NUM_VOICES];
    
    // Using seperate objects for pairs to easily maintain phase relation
    tMBSawPulse sawPaired[MAX_NUM_VOICES];
    tMBSineTri sinePaired[MAX_NUM_VOICES];
    
    
    tWaveOscS wave[MAX_NUM_VOICES];
    
   
    std::unique_ptr<SmoothedParameter> filterSend;
    std::atomic<float>* isHarmonic_raw;
    std::atomic<float>* isStepped_raw;
    std::atomic<float>* isSync_raw;
    std::atomic<float>* syncType_raw;
    std::atomic<float>* afpShapeSet;
    OscShapeSet currentShapeSet = OscShapeSetNil;
    
    float outSamples[2][MAX_NUM_VOICES];
    
    File waveTableFile;
    bool loadingTables = false;
    
    Oscillator *syncSource;
};

//==============================================================================

class LowFreqOscillator : public AudioComponent,
                          public MappingSourceModel
{
public:
    //==============================================================================
    LowFreqOscillator(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~LowFreqOscillator();
    void setParams() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    float tick();
    void loadAll(int);
    
    //==============================================================================
    void noteOn(int voice, float velocity);
    void noteOff(int voice, float velocity);
    
    //==============================================================================
    LFOShapeSet getCurrentShapeSet() { return currentShapeSet; }
    
private:
    
    void (LowFreqOscillator::*shapeTick)(float& sample, int v);
    void sawSquareTick(float& sample, int v);
    void sineTriTick(float& sample, int v);
    void userTick(float& sample, int v);
    void sawTick(float& sample, int v);
    void pulseTick(float& sample, int v);
    void sineTick(float& sample, int v);
    void triTick(float& sample, int v);
    void setRate(int v, float rate);
    void setShape(int v, float shape);
    
    RangedAudioParameter* sync;
    
    tIntPhasor saw[MAX_NUM_VOICES];
    tSquareLFO pulse[MAX_NUM_VOICES];
    tCycle sine[MAX_NUM_VOICES];
    tTriLFO tri[MAX_NUM_VOICES];
    
    // Using seperate objects for pairs to easily maintain phase relation
    tSineTriLFO sineTri[MAX_NUM_VOICES];
    tSawSquareLFO sawSquare[MAX_NUM_VOICES];
    
    tWaveOscS wave[MAX_NUM_VOICES];
    
    
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
    NoiseGenerator(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~NoiseGenerator();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float output[][MAX_NUM_VOICES]);
    void loadAll(int);
private:
    // noise tilt
    tVZFilter shelf1[MAX_NUM_VOICES], shelf2[MAX_NUM_VOICES], bell1[MAX_NUM_VOICES];
    tNoise noise[MAX_NUM_VOICES];

    
    std::unique_ptr<SmoothedParameter> filterSend;
    
    float* sourceValue;
};
