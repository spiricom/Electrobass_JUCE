/*
  ==============================================================================

    Output.cpp
    Created: 17 Jun 2021 10:57:13pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Output.h"
#include "PluginProcessor.h"

//==============================================================================
Output::Output(const String& n, ESAudioProcessor& p,
               AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cOutputParams, false)
{
    master = std::make_unique<SmoothedParameter>(processor, vts, "Master", -1);
}

Output::~Output()
{
    
}

void Output::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

void Output::frame()
{
    for (int i = 0; i < params.size(); ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            lastValues[i][v] = values[i][v];
            values[i][v] = ref[i][v]->skip(currentBlockSize);
        }
    }
    sampleInBlock = 0;
}

void Output::tick(float input[NUM_STRINGS], float output[2], int numChannels)
{
    float a = sampleInBlock * invBlockSize;
    float m = master->tickNoHooks();
    
    for (int v = 0; v < NUM_STRINGS; ++v)
    {
        float amp = values[OutputAmp][v]*a + lastValues[OutputAmp][v]*(1.f-a);
        float pan = values[OutputPan][v]*a + lastValues[OutputPan][v]*(1.f-a);
        amp = amp < 0.f ? 0.f : amp;
        pan = LEAF_clip(0.f, pan, 1.f);
        
        float sample = input[v] * amp * m;
        
        if (numChannels > 1)
        {
            output[0] += sample*(1.f-pan);
            output[1] += sample*pan;
        }
        else output[0] += sample;
    }
    
    sampleInBlock++;
}
