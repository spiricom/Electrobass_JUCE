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
                               AudioProcessorValueTreeState& vts, StringArray s) :
processor(p),
vts(vts),
name(n),
paramNames(s)
{
    for (int i = 0; i < paramNames.size(); ++i)
    {
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            params[i]->add(new SmoothedParameter(p, vts, name + paramNames[i]));
        }
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
