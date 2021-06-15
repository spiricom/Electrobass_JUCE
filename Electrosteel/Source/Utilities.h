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
    sourceName(""),
    hook(&value0),
    min(0.0f),
    length(0.0f),
    operation(HookAdd)
    {
    }
    
    ParameterHook(String sourceName, float* hook, float min, float max, HookOperation op) :
    sourceName(sourceName),
    hook(hook),
    min(min),
    length(max-min),
    operation(op)
    {
    }
    
    ~ParameterHook() {};
    //==============================================================================
    float apply(float input)
    {
        float hookValue = (*hook * length + min);
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
        return ((*hook * length) + min);
    }

    String sourceName;
    float* hook;
    float min, length;
    HookOperation operation;

private:
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
    
    float skip(int numSamples)
    {
        float target = raw->load() +
        hooks[0].getValue() + hooks[1].getValue() + hooks[2].getValue();
        smoothed.setTargetValue(target);
        return value = smoothed.skip(numSamples);
    }
    
    float get()
    {
        return value;
    }
    
    float* getValuePointer()
    {
        return &value;
    }
    
    ParameterHook& getHook(int index)
    {
        return hooks[index];
    }
    
    void setHook(const String& sourceName, int index, float* hook, float min, float max, HookOperation op)
    {
        hooks[index].sourceName = sourceName;
        hooks[index].hook = hook;
        hooks[index].min = min;
        hooks[index].length = max-min;
        hooks[index].operation = op;
    }
    
    void resetHook(int index)
    {
        hooks[index].sourceName = "";
        hooks[index].hook = &value0;
        hooks[index].min = 0.0f;
        hooks[index].length = 0.0f;
        hooks[index].operation = HookAdd;
    }
    
    void setRange(int index, float min, float max)
    {
        hooks[index].min = min;
        hooks[index].length = max-min;
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
    AudioComponent(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&, StringArray, bool);
    ~AudioComponent();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    OwnedArray<SmoothedParameter>& getParameterArray(int p);
    
    bool isEnabled();
    
    String name;
        
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    OwnedArray<OwnedArray<SmoothedParameter>> params;
    StringArray paramNames;
    
    std::atomic<float>* afpEnabled;
    
    float passTick(float sample) { return sample; }
    
    double currentSampleRate;
    int currentSamplesPerBlock;
    
    bool toggleable;
};

//==============================================================================
//==============================================================================

class Output : public AudioComponent
{
public:
    //==============================================================================
    Output(const String&, ESAudioProcessor&, AudioProcessorValueTreeState&);
    ~Output();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    //==============================================================================
    void tick(int v, float input, float* output, int numChannels);
    
private:
    
    SmoothedParameter* ref[OutputParamNil][NUM_STRINGS];
    std::unique_ptr<SmoothedParameter> master;
};

