/*
  ==============================================================================

    Utilities.h
    Created: 17 Mar 2021 3:07:50pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    }
    ~SmoothedParameter() {};
    
    operator float()
    {
        smoothed.setTargetValue(*parameter);
        return smoothed.getNextValue();
    }
    
private:
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothed;
    std::atomic<float>* parameter;
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
        
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    Array<SmoothedParameter> params;
    String name;
    StringArray paramNames;
    
    static float passTick(float sample) { return sample; }
};
