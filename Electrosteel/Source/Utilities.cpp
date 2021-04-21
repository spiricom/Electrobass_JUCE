/*
  ==============================================================================

    Utilities.cpp
    Created: 17 Mar 2021 4:28:36pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Utilities.h"
#include "PluginProcessor.h"

AudioComponent::AudioComponent(const String& n, ESAudioProcessor& p,
                               AudioProcessorValueTreeState& vts, StringArray s) :
processor(p),
vts(vts),
name(n),
paramNames(s)
{
    for (int v = 0; v < NUM_VOICES; ++v)
    {
        for (int i = 0; i < paramNames.size(); ++i)
        {
            params[v].add(new SmoothedParameter(vts, name + paramNames[i]));
        }
    }
}

AudioComponent::~AudioComponent()
{
}

void AudioComponent::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

SmoothedParameter* AudioComponent::getParameter(int voice, int p)
{
    return params[voice][p];
}
