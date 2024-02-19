/*
  ==============================================================================

    Filters.h
    Created: 17 Mar 2021 3:10:49pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "../Constants.h"
#include "Utilities.h"

class ElectroAudioProcessor;

class Filter : public AudioComponent
{
public:
    //==============================================================================
    Filter(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&);
    ~Filter();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void frame();
    void tick(float* samples);
    void loadAll(int);
    void setParams() override;
    
private:
    
    void (Filter::*filterTick)(float& sample, int v, float cutoff, float q, float morph, float gain);
    void lowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void highpassTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void bandpassTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void diodeLowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void LadderLowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void VZlowshelfTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void VZhighshelfTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void VZpeakTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void VZbandrejectTick(float& sample, int v, float cutoff, float q, float morph, float gain);
    void setQ(float q, int v);
    void setGain(float gain, int v);
    
    tDiodeFilter diodeFilters[MAX_NUM_VOICES];
    tVZFilterBell VZfilterPeak[MAX_NUM_VOICES];
    tVZFilterLS VZfilterLS[MAX_NUM_VOICES];
    tVZFilterHS VZfilterHS[MAX_NUM_VOICES];
    tVZFilterBR VZfilterBR[MAX_NUM_VOICES];
    tSVF lowpass[MAX_NUM_VOICES];
    tSVF highpass[MAX_NUM_VOICES];
    tSVF bandpass[MAX_NUM_VOICES];
    tLadderFilter Ladderfilter[MAX_NUM_VOICES];
    bool setAllVariables = false;
    float G = 1.0f;
    std::atomic<float>* afpFilterType;
    FilterType currentFilterType = FilterTypeNil;
};
