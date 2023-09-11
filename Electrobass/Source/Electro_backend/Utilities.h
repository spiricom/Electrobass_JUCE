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
    void setHookRange(int index, float min, float max, bool isBipolar);
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
    bool isSkewed;
    float value0 = 0.0f;
    float value1 = 1.0f;
};

class SkewedParameter : public SmoothedParameter
{
public:
    SkewedParameter(ElectroAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                 String paramId, float start, float end, float center)
    : SmoothedParameter(processor, vts, paramId)
    {
        isSkewed = true;
         LEAF_generate_table_skew_non_sym(skewTable, start, end, center, 2048);
    }
    
    float tick() override
    {
        
        // Well defined inter-thread behavior PROBABLY shouldn't be an issue here, so
        // the atomic is just slowing us down. memory_order_relaxed seems fastest, marginally
        removeMe = false;
        float target = parameter->getValue();
        //bool isSmoothed = false;
        for (int i = 0; i < numSmoothedHooks; ++i)
        {
            target += hooks[smoothedHooks[i]].getValue();
        }
        //DBG("Target" + String(target));
        smoothed.setTargetValue(scale(target) );
        value = smoothed.getNextValue();
        for (int i = 0; i < numNonSmoothedHooks; ++i)
        {
            value += scale(hooks[nonSmoothedHooks[i]].getValue());
        }
        if ((numSmoothedHooks == 0) && (numNonSmoothedHooks == 0))
        {
            if ((value >= (target - 0.0001f)) && (value <= (target + 0.0001f)))
            {
                removeMe = true;
            }
        }
        return value;
        
    }
    
private:
    float skewTable[2048];
    float scale(float input)
    {
        //lookup table for env times
        if (isnan(input))
        {
            input = 0.0f;
        }
        input = LEAF_clip(0.0f, input, 1.0f);
        //scale to lookup range
        input *= 2047.0f;
        int inputInt = (int)input;
        float inputFloat = (float)inputInt - input;
        int nextPos = LEAF_clipInt(0, inputInt + 1, 2047);
        return (skewTable[inputInt] * (1.0f - inputFloat)) + (skewTable[nextPos] * inputFloat);

        //return input;
    }
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
