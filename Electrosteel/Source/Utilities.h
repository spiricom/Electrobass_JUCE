/*
  ==============================================================================

    Utilities.h
    Created: 17 Mar 2021 3:07:50pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"

class ESAudioProcessor;

//==============================================================================
class SmoothedParameter
{
public:
    //==============================================================================
    SmoothedParameter() = default;
    SmoothedParameter(std::atomic<float>* p)
    {
        parameter = p;
        mHook = &value1;
        aHook = &value0;
        addMin = 0.0f;
        addMax = 1.0f;
    }
    ~SmoothedParameter() {};
    
    float tick()
    {
        float target = (*parameter * *mHook) + (*aHook * (addMax - addMin) + addMin);
        smoothed.setTargetValue(target);
        return value = smoothed.getNextValue();
    }
    
    float* getValuePointer()
    {
        return &value;
    }
    
    void setMultiplyHook(float* hook)
    {
        mHook = hook;
    }
    
    void setAddHook(float* hook, float min, float max)
    {
        aHook = hook;
        addMin = min;
        addMax = max;
    }
    
private:
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* parameter;
    float value;
    const float* mHook;
    const float* aHook;
    float addMin, addMax;
};

//==============================================================================
//==============================================================================

class AudioComponent
{
public:
    //==============================================================================
    AudioComponent(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray);
    ~AudioComponent();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    SmoothedParameter* getParameter(int i, int voice);
        
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    OwnedArray<SmoothedParameter> params[NUM_VOICES];
    String name;
    StringArray paramNames;
    
    static float passTick(float sample) { return sample; }
};
