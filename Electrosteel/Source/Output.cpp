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
        pan = LEAF_clip(-1.f, pan, 1.f);
        
        float sample = input[v] * amp * m;
        
        // Porting over some code from
        // https://github.com/juce-framework/JUCE/blob/master/modules/juce_dsp/processors/juce_Panner.cpp
        
        if (numChannels > 1)
        {
            float normPan = 0.5f * (pan+1.f);
            
            // balanced
            //        float lg = jmin(0.5f, 1.f - normPan);
            //        float rg = jmin(0.5f, normPan);
            //        float boost = 2.f;
            
            // linear
            //        float lg = 1.f - normPan;
            //        float rg = normPan;
            //        float boost = 2.f;
            
            // sin3dB
            float lg = std::sinf(0.5f * PI * (1.f - normPan));
            float rg = std::sinf(0.5f * PI * normPan);
            float boost = LEAF_SQRT2;
            
            // sin6dB
            //        float lg = std::powf(std::sin(0.5f * PI * (1.f - normPan)), 2.f);
            //        float rg = std::powf(std::sinf(0.5f * PI * normPan), 2.f);
            //        float boost = 2.f;
            
            // squareRoot3dB
            //        float lg = std::sqrtf(1.f - normPan);
            //        float rg = std::sqrtf(normPan);
            //        float boost = LEAF_SQRT2;
            
            output[0] += sample*lg*boost;
            output[1] += sample*rg*boost;
        }
        else output[0] += sample;
    }
    
    sampleInBlock++;
}
