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
typedef enum HookOperation
{
    HookAdd = 0,
    HookMultiply,
    HookNil
} HookOperation;

class ParameterHook
{
public:
    //==============================================================================
    ParameterHook(float* hook, float min, float max, HookOperation op) :
    hook(hook),
    min(min),
    max(max),
    operation(op)
    {
        
    }
    ~ParameterHook() {};
    //==============================================================================
    float apply(float input)
    {
        float hookValue = (*hook * (max - min) + min);
        if (operation == HookAdd)
        {
            return input + hookValue;
        }
        if (operation == HookMultiply)
        {
            return input * hookValue;
        }
        return input;
    }

    float* hook;
    float min, max;
    HookOperation operation;
};

//==============================================================================
class SmoothedParameter
{
public:
    //==============================================================================
    SmoothedParameter() = default;
    SmoothedParameter(std::atomic<float>* p)
    {
        parameter = p;
    }
    ~SmoothedParameter() {};
    //==============================================================================
    float tick()
    {
        float target = *parameter;
        for (auto hook : hooks)
        {
            target = hook.apply(target);
        }
        smoothed.setTargetValue(target);
        return value = smoothed.getNextValue();
    }
    
    float* getValuePointer()
    {
        return &value;
    }
    
    void addHook(float* hook, float min, float max, HookOperation op)
    {
        hooks.add(ParameterHook(hook, min, max, op));
    }
    
    void moveHook(int index, int newIndex)
    {
        hooks.move(index, newIndex);
    }
    
private:
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* parameter;
    float value;
    Array<ParameterHook> hooks;
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
