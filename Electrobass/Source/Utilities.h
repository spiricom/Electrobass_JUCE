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
    ParameterHook() = default;
    
    ParameterHook(String sourceName, float* hook, float min, float max,
                  String scalarName, float* scalar) :
    sourceName(sourceName),
    hook(hook),
    min(min),
    length(max-min),
    scalarName(scalarName),
    scalar(scalar)
    {
    }
    
    ~ParameterHook() {};
    
    //==============================================================================
    inline float getValue()
    {
        // Significant bottleneck; gets called for each hook for each param
        // for each voice every tick so if there's any possible
        // optimization here it should be impactful
        return ((*hook * length) + min) * *scalar;
    }

    String sourceName;
    float* hook;
    float min, length;
    String scalarName;
    float* scalar;
};

//==============================================================================
//==============================================================================
class SmoothedParameter
{
public:
    //==============================================================================
//    SmoothedParameter() = default;
    SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                      String paramId);
    ~SmoothedParameter() {};
    //==============================================================================
    float tick();
    float tickNoHooks();
    float tickNoSmoothing();
    float tickNoHooksNoSmoothing();
    void tickSkews();
    void tickSkewsNoHooks();
    void tickSkewsNoSmoothing();
    void tickSkewsNoHooksNoSmoothing();
    
    float skip(int numSamples);

    float get();
    float get(int i);
    float** getValuePointerArray();
    float** getValuePointerArray(int i);
    
    ParameterHook& getHook(int index);
    void setHook(const String& sourceName, int index,
                 const float* hook, float min, float max);
    void setHookRange(int index, float min, float max);
    void setHookScalar(const String& scalarName, int index, float* scalar);
    void resetHook(int index);
    void resetHookScalar(int index);
    
    float getStart();
    float getEnd();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
    float getInvSkew() { return 1.f/range.skew; }
    NormalisableRange<float>& getRange() { return range; }
    float getRawValue() { return *raw; }
    
private:
    ESAudioProcessor& processor;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* raw;
    RangedAudioParameter* parameter;
    NormalisableRange<float> range;
    float value = 0.f;
    float* valuePointer = &value;
    float values[MAX_NUM_UNIQUE_SKEWS];
    float* valuePointers[MAX_NUM_UNIQUE_SKEWS];
    ParameterHook hooks[3];
    int numActiveHooks = 0;
    int whichHooks[3];
    
    float value0 = 0.0f;
    float value1 = 1.0f;
};

//==============================================================================
//==============================================================================

class MappingSourceModel
{
public:
    MappingSourceModel(ESAudioProcessor& p, const String &name,
                       bool perVoice, bool bipolar, Colour colour);
    ~MappingSourceModel();
    
    bool isBipolar() { return bipolar; }

    float** getValuePointerArray(int i);
    int getNumSourcePointers();
    
    String name;
    float** sources[MAX_NUM_UNIQUE_SKEWS];
    int numSourcePointers;
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

    void setMapping(MappingSourceModel* source, float end, bool sendChangeEvent);
    void setMappingRange(float end, bool sendChangeEvent,
                         bool sendListenerNotif, bool updateSlider);
    void setMappingScalar(MappingSourceModel* source, bool sendChangeEvent);
    void removeMapping(bool sendChangeEvent);
    void removeScalar(bool sendChangeEvent);
    
    bool isBipolar() { return bipolar; }
    
    std::function<void(bool, bool)> onMappingChange = nullptr;

    ESAudioProcessor& processor;
    
    String name;
    MappingSourceModel* currentSource = nullptr;
    MappingSourceModel* currentScalarSource = nullptr;
    OwnedArray<SmoothedParameter>& targetParameters;
    int index;
    float start, end;
    bool bipolar;
    float invSkew;
    
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
    SmoothedParameter* quickParams[10][NUM_STRINGS];
    
    OwnedArray<MappingTargetModel> targets;
    
    std::atomic<float>* afpEnabled;
    bool enabled;
    
    double currentSampleRate = 0.;
    int currentBlockSize = 0;
    float invBlockSize = 0.f;
    
    int sampleInBlock;
    
    bool toggleable;
};
