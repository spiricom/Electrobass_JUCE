/*
  ==============================================================================

    Envelopes.cpp
    Created: 17 Mar 2021 4:17:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Envelopes.h"
#include "PluginProcessor.h"

//==============================================================================
Envelope::Envelope(const String& n, ESAudioProcessor& p,
                   AudioProcessorValueTreeState& vts, StringArray s) :
AudioComponent(n, p, vts, s)
{
    //exponential buffer rising from 0 to 1
    LEAF_generate_exp(expBuffer, 1000.0f, -1.0f, 0.0f, -0.0008f, EXP_BUFFER_SIZE);
    
    // exponential decay buffer falling from 1 to
    LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE);
    
    expBufferSizeMinusOne = EXP_BUFFER_SIZE - 1;
    decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tADSRT_init(&envs[i], expBuffer[0] * 8192.0f,
                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &processor.leaf);
        tADSRT_setLeakFactor(&envs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
    }
}

Envelope::~Envelope()
{
    
}

void Envelope::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tADSRT_setSampleRate(&envs[i], sampleRate);
    }
}

void Envelope::tick()
{
    float attack = params[EnvelopeAttack];
    float decay = params[EnvelopeDecay];
    float sustain = params[EnvelopeSustain];
    float release = params[EnvelopeRelease];
    float leak = params[EnvelopeLeak];
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tADSRT_setAttack(&envs[i], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setDecay(&envs[i], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setSustain(&envs[i], sustain);
        tADSRT_setRelease(&envs[i], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setLeakFactor(&envs[i], ((1.0f - leak) * 0.00005f) + 0.99995f);
        
        value[i] = tADSRT_tick(&envs[i]);
    }
}

void Envelope::noteOn(int voice, float velocity)
{
    tADSRT_on(&envs[voice], velocity);
}

void Envelope::noteOff(int voice, float velocity)
{
    tADSRT_off(&envs[voice]);
}

float* Envelope::getValue(int voice)
{
    return &value[voice];
}
