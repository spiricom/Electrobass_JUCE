/*
  ==============================================================================

    Utilities.cpp
    Created: 17 Mar 2021 4:28:36pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Utilities.h"
#include "PluginProcessor.h"

SmoothedParameter::SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts, String paramId) :
processor(processor)
{
    raw = vts.getRawParameterValue(paramId);
    parameter = vts.getParameter(paramId);
    for (int i = 0; i < 3; ++i) hooks[i] = ParameterHook(&value0, 0.0f, 0.0f, HookAdd);
    processor.params.add(this);
}

//==============================================================================

AudioComponent::AudioComponent(const String& n, ESAudioProcessor& p,
                               AudioProcessorValueTreeState& vts, StringArray s,
                               bool toggleable) :
processor(p),
vts(vts),
name(n),
paramNames(s),
toggleable(toggleable)
{
    for (int i = 0; i < paramNames.size(); ++i)
    {
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            params[i]->add(new SmoothedParameter(p, vts, name + paramNames[i]));
        }
    }
    
    if (toggleable)
    {
        afpEnabled = vts.getRawParameterValue(n);
    }
    else
    {
        afpEnabled = nullptr;
    }
}

AudioComponent::~AudioComponent()
{
}

void AudioComponent::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;
}

OwnedArray<SmoothedParameter>& AudioComponent::getParameter(int p)
{
    return *params[p];
}

bool AudioComponent::isEnabled()
{
    return afpEnabled == nullptr || *afpEnabled > 0;
}

//==============================================================================
Output::Output(const String& n, ESAudioProcessor& p,
               AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cOutputParams, false)
{
    for (int i = 0; i < OutputParamNil; ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    master = std::make_unique<SmoothedParameter>(processor, vts, "Master");
}

Output::~Output()
{
    
}

void Output::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

void Output::tick(int v, float input, float* output, int numChannels)
{
    float amp = ref[OutputAmp][v]->tick();
    float pan = ref[OutputPan][v]->tick();
    
    input *= amp * master->tick();;
    
    if (numChannels > 1)
    {
        output[0] += 2.f*input*(1.f-pan);
        output[1] += 2.f*input*pan;
    }
    else output[0] += input;
}
