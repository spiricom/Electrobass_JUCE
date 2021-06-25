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
class ParameterHook
{
public:
    //==============================================================================
    ParameterHook() :
    sourceName(""),
    hook(&value0),
    buffered(false),
    size(1),
    min(0.0f),
    length(0.0f)
    {
    }
    
    ParameterHook(String sourceName, float* hook, bool buffered,
                  int size, float min, float max) :
    sourceName(sourceName),
    hook(hook),
    buffered(buffered),
    size(size),
    min(min),
    length(max-min)
    {
    }
    
    ~ParameterHook() {};
    
    //==============================================================================
    float getValue(int i)
    {
        return ((hook[i%size] * length) + min);
    }

    String sourceName;
    float* hook;
    bool buffered;
    int size;
    float min, length;

private:
    float value0 = 0.0f;
};

//==============================================================================
//==============================================================================
class SmoothedParameter
{
public:
    //==============================================================================
//    SmoothedParameter() = default;
    SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                      String paramId, int voice);
    ~SmoothedParameter() {};
    //==============================================================================
    float tick(int i);
    float tickNoHooks();
float skip(int numSamples);
    
    float get();
    float** getValuePointerArray();
    
    ParameterHook& getHook(int index);
    void setHook(const String& sourceName, int index,
                 const float* hook, int size, float min, float max);
    void resetHook(int index);
    void updateHook(int index, const float* hook);
    
    void setRange(int index, float min, float max);
    
    float getStart();
    float getEnd();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
    int getVoice() { return voice; }
    
private:
    ESAudioProcessor& processor;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* raw;
    RangedAudioParameter* parameter;
    float value = 0.0f;
    float* valuePointer = &value;
    float value0 = 0.0f;
    ParameterHook hooks[3];
    int voice;
};

//==============================================================================
//==============================================================================

class MappingSourceModel
{
public:
    MappingSourceModel(ESAudioProcessor& p, const String &name, float** source,
                       bool perVoice, bool buffered, bool bipolar, Colour colour);
    ~MappingSourceModel();
    
    bool isBipolar() { return bipolar; }

    float** getValuePointerArray();
    int getNumSourcePointers();
    int getBufferSize();
    
    String name;
    float** source;
    int numSourcePointers;
    bool buffered;
    bool bipolar;
    Colour colour;
    
private:
    ESAudioProcessor& modelProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingSourceModel)
};

//==============================================================================
//==============================================================================

class MappingTargetModel
{
public:
    
    MappingTargetModel(ESAudioProcessor& p, const String &name,
                       OwnedArray<SmoothedParameter>& targetParameters, int index);
    ~MappingTargetModel();
    
    void prepareToPlay();

    bool setMapping(MappingSourceModel* source, float end, bool sendChangeEvent);
    void removeMapping(bool sendChangeEvent);
    void setMappingRange(float end, bool sendChangeEvent);
    
    bool isBipolar() { return bipolar; }
    
    std::function<void(bool)> onMappingChange = nullptr;

    ESAudioProcessor& processor;
    
    String name;
    MappingSourceModel* currentSource;
    OwnedArray<SmoothedParameter>& targetParameters;
    int index;
    bool bipolar;
    float value;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingTargetModel)
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
    
    bool isToggleable() { return toggleable; }
    bool isEnabled();
    
    String& getName() { return name; }
    StringArray& getParamNames() { return paramNames; }
    
    MappingTargetModel* getTarget(int paramId, int index);
    
protected:
    String name;
        
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    OwnedArray<OwnedArray<SmoothedParameter>> params;
    StringArray paramNames;
    
    // First size needs to be at least the greatest number of params for any component
    SmoothedParameter* ref[10][NUM_STRINGS];
    
    OwnedArray<MappingTargetModel> targets;
    
    std::atomic<float>* afpEnabled;
    bool enabled;
    
    double currentSampleRate = 0.;
    int currentBlockSize = 0;
    float invBlockSize = 0.f;
    
    int sampleInBlock;
    
    float lastValues[10][NUM_STRINGS];
    float values[10][NUM_STRINGS];
    
    bool toggleable;
};
