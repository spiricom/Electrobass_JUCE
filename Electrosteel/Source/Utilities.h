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
    ParameterHook() :
    hook(&value0),
    min(0.0f),
    max(0.0f),
    operation(HookAdd)
    {
    }
    
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
//        if (operation == HookAdd)
//        {
            return input + hookValue;
//        }
//        if (operation == HookMultiply)
//        {
//            return input * hookValue;
//        }
//        return input;
    }
    
    float getValue()
    {
        return (*hook * (max - min) + min);
    }

    float* hook;
    float min, max;
    HookOperation operation;
    float value0 = 0.0f;
};

//==============================================================================
class SmoothedParameter
{
public:
    //==============================================================================
//    SmoothedParameter() = default;
    SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts, String paramId);
    ~SmoothedParameter() {};
    //==============================================================================
    float tick()
    {
        float target = raw->load() +
        hooks[0].getValue() + hooks[1].getValue() + hooks[2].getValue();
        smoothed.setTargetValue(target);
        return value = smoothed.getNextValue();
    }
    
    float* getValuePointer()
    {
        return &value;
    }
    
    void setHook(int index, float* hook, float min, float max, HookOperation op)
    {
        hooks[index].hook = hook;
        hooks[index].min = min;
        hooks[index].max = max;
        hooks[index].operation = op;
    }
    
    void resetHook(int index)
    {
        hooks[index].hook = &value0;
        hooks[index].min = 0.0f;
        hooks[index].max = 0.0f;
        hooks[index].operation = HookAdd;
    }
    
    void setRange(int index, float min, float max)
    {
        hooks[index].min = min;
        hooks[index].max = max;
    }
    
    float getStart() { return parameter->getNormalisableRange().start; }
    float getEnd() { return parameter->getNormalisableRange().end; }
    
    void setSampleRate(double sampleRate)
    {
        smoothed.reset(sampleRate, 0.010);
    }
    
private:
    
    ESAudioProcessor& processor;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* raw;
    RangedAudioParameter* parameter;
    float value = 0.0f;
    float value0 = 0.0f;
    ParameterHook hooks[3];
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
    OwnedArray<SmoothedParameter>& getParameter(int p);
        
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    OwnedArray<OwnedArray<SmoothedParameter>> params;
    String name;
    StringArray paramNames;
    
    static float passTick(float sample) { return sample; }
};
