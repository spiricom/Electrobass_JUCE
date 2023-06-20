/*
  ==============================================================================

    Output.cpp
    Created: 17 Jun 2021 10:57:13pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Output.h"
#include "../PluginProcessor.h"

//==============================================================================
Output::Output(const String& n, ElectroAudioProcessor& p,
               AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cOutputParams, false)
{
    setParams();
    master = std::make_unique<SmoothedParameter>(processor, vts, "Master");
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tSVF_init(&lowpass[i], SVFTypeLowpass, 19000.f, 0.3f, &processor.leaf);
    }
    
}

Output::~Output()
{
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tSVF_free(&lowpass[i]);
    }
}

void Output::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    
}

void Output::frame()
{
    sampleInBlock = 0;
}

void Output::tick(float input[MAX_NUM_VOICES])
{
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float amp = quickParams[OutputAmp][v]->read();
        amp = amp < 0.f ? 0.f : amp;
        input[v] = input[v] * amp;
        // Porting over some code from
        // https://github.com/juce-framework/JUCE/blob/master/modules/juce_dsp/processors/juce_Panner.cpp
        
    }

    sampleInBlock++;
}

void Output::tickLowpass(float input [MAX_NUM_VOICES])
{
    
    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float midiCutoff = quickParams[OutputTone][v]->read();
        lowpassTick(input[v], v, midiCutoff);
    }
}

void Output::lowpassTick(float& sample, int v, float cutoff)
{
    float in = LEAF_clip(0.0f,(((cutoff* 70.0f) + 58.0f)-16.0f) * 35.929824561403509f,4095.f);
    tSVF_setFreqFast(&lowpass[v], in);
    sample = tSVF_tick(&lowpass[v], sample);
}
