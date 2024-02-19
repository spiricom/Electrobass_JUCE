/*
  ==============================================================================

    Utilities.h
    Created: 17 Mar 2021 3:07:50pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Constants.h"

class ElectroAudioProcessor;

//==============================================================================
class ParameterHook
{
public:
    //==============================================================================
    ParameterHook() = default;
    
    ParameterHook(String sourceNameIn, float* hookIn, float minIn, float maxIn,
                  String scalarNameIn, float* scalarIn) :
    sourceName(sourceNameIn),
    hook(hookIn),
    min(minIn),
    length(maxIn-minIn),
    scalarName(scalarNameIn),
    scalar(scalarIn)
    {
    }
    
    ~ParameterHook() {}
    
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
    SmoothedParameter(ElectroAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                      String paramId);
    ~SmoothedParameter() {}
    //==============================================================================
    virtual float tick();
    float tickNoHooks();
    float tickNoHooksNoSmoothing();

    float get();
    
    ParameterHook& getHook(int index);
    void setHook(const String& sourceName, int index,
                 const float* hook, float min, float max);
    float setHookRange(int index, float min, float max, bool isBipolar);
    void setHookScalar(const String& scalarName, int index, float* scalar);
    void resetHook(int index);
    void resetHookScalar(int index);
    
    float getStart();
    float getEnd();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
    float getInvSkew() { return 1.f/range.skew; }
    NormalisableRange<float>& getRange() { return range; }
    float getRawValue() { return *raw; }
    String getName() {return name;}
    bool getRemoveMe(){return removeMe;}
    void setRemoveMe(bool val){removeMe = val;}

    float read(){return value;}
    void setValueToRaw()
    {
        value = *raw;
//        parameter->setValueNotifyingHost(parameter->convertFrom0to1(value));
        
    }
   float* getValuePointer()
    {
       return valuePointer;
    }
protected:
    ElectroAudioProcessor& processor;
    String name;
    bool removeMe;
    std::atomic<float>* raw;
    RangedAudioParameter* parameter;
    NormalisableRange<float> range;
    float value = 0.f;
    float* valuePointer = &value;
    ParameterHook hooks[3];
    int numSmoothedHooks = 0;
    int numNonSmoothedHooks = 0;
    int smoothedHooks[3];
    int nonSmoothedHooks[3];
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    float value0 = 0.0f;
    float value1 = 1.0f;
};


//==============================================================================
//==============================================================================

class MappingSourceModel
{
public:
    MappingSourceModel(ElectroAudioProcessor& p, const String &name,
                       bool perVoice, bool bipolar, Colour colour);
    ~MappingSourceModel();
    
    bool isBipolar() { return bipolar; }

    float* getValuePointerArray();
    int getNumSourcePointers();
    void setValue(float f)
    {
        *source = f;
    }
    String name;

    int numSourcePointers;
    bool bipolar;
    Colour colour;
    float* source;
private:
    ElectroAudioProcessor& modelProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingSourceModel)
};

//==============================================================================
//==============================================================================

class MappingTargetModel
{
public:
    
    MappingTargetModel(ElectroAudioProcessor& p, const String &name, 
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

    ElectroAudioProcessor& processor;
    
    String name;
    MappingSourceModel* currentSource = nullptr;
    MappingSourceModel* currentScalarSource = nullptr;
    OwnedArray<SmoothedParameter>& targetParameters; // per voice
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
    AudioComponent(const String&, ElectroAudioProcessor&, AudioProcessorValueTreeState&, StringArray, bool);
    ~AudioComponent();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void virtual setParams();

    //==============================================================================
    OwnedArray<SmoothedParameter>& getParameterArray(int p);
    int getParamArraySize() {return params.size();}
    bool isToggleable() { return toggleable; }
    bool isEnabled() { return enabled; }

    String& getName() { return name; }
    StringArray& getParamNames() { return paramNames; }
    
    MappingTargetModel* getTarget(int paramId, int index);
    ElectroAudioProcessor& processor;
protected:
    String name;
        
    
    AudioProcessorValueTreeState& vts;
    OwnedArray<OwnedArray<SmoothedParameter>> params;
    StringArray paramNames;
    
    // First size needs to be at least the greatest number of params for any component
    SmoothedParameter* quickParams[10][MAX_NUM_VOICES];
    
    OwnedArray<MappingTargetModel> targets;
    
    std::atomic<float>* afpEnabled;
    bool enabled;
    
    double currentSampleRate = 0.;
    int currentBlockSize = 0;
    float invBlockSize = 0.f;
    
    int sampleInBlock;
    bool isOn;
    bool toggleable;
};
