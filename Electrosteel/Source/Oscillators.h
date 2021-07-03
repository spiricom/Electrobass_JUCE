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
    void frame();
    void tick(float output[][NUM_STRINGS]);
    
    //==============================================================================
    void addWaveTables(File& file);
    void clearWaveTables();
    void waveTablesChanged();
    
    void setWaveTableFile(File table) { waveTableFile = table; }
    File& getWaveTableFile() { return waveTableFile; }
    void setLoadingTables(bool loading) { loadingTables = loading; }
    
private:
    
    void (Oscillator::*shapeTick)(float& sample, int v, float freq, float shape);
    void sawPulseTick(float& sample, int v, float freq, float shape);
    void userTick(float& sample, int v, float freq, float shape);
    
    tMBSaw saw[NUM_STRINGS];
    tMBPulse pulse[NUM_STRINGS];
    
    tWaveTable* tables = nullptr;
    tWaveOsc wave[NUM_STRINGS];

    std::unique_ptr<SmoothedParameter> filterSend;
    
    std::atomic<float>* afpShapeSet;
    OscShapeSet currentShapeSet;
    
    File waveTableFile;
    OwnedArray<AudioBuffer<float>> waveTables;
    int lastNumWaveTables = 0;
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
    void addWaveTables(File& file);
    void clearWaveTables();
    void waveTablesChanged();
    
    void setWaveTableFile(File table) { waveTableFile = table; }
    File& getWaveTableFile() { return waveTableFile; }
    void setLoadingTables(bool loading) { loadingTables = loading; }
    
private:
    RangedAudioParameter* sync;
    
    tCycle lfo[NUM_STRINGS];
    
    tWaveTable* tables = nullptr;
    tWaveOsc wave[NUM_STRINGS];
    
    float* lfoValues[NUM_STRINGS];
    float phaseReset;
    
    File waveTableFile;
    OwnedArray<AudioBuffer<float>> waveTables;
    int lastNumWaveTables = 0;
    bool loadingTables = false;
};
