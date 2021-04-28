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
    // Trying to make it as fast as possible to access the SmoothedParameters
    // so we'll put pointers to them in a plain array instead of the OwnedArrays
    for (int i = 0; i < EnvelopeParamNil; ++i)
    {
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
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
        value[i] = 0.0f;
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

void Envelope::frame()
{
    for (int v = 0; v < NUM_VOICES; v++)
    {
        float attack = ref[EnvelopeAttack][v]->skip(currentSamplesPerBlock);
        float decay = ref[EnvelopeDecay][v]->skip(currentSamplesPerBlock);
        float sustain = ref[EnvelopeSustain][v]->skip(currentSamplesPerBlock);
        float release = ref[EnvelopeRelease][v]->skip(currentSamplesPerBlock);
        float leak = ref[EnvelopeLeak][v]->skip(currentSamplesPerBlock);

        tADSRT_setAttack(&envs[v], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setDecay(&envs[v], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setSustain(&envs[v], sustain);
        tADSRT_setRelease(&envs[v], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setLeakFactor(&envs[v], ((1.0f - leak) * 0.00005f) + 0.99995f);
    }
}

void Envelope::tick()
{
    for (int v = 0; v < NUM_VOICES; v++)
    {
//        float attack = ref[EnvelopeAttack][v]->tick();
//        float decay = ref[EnvelopeDecay][v]->tick();
//        float sustain = ref[EnvelopeSustain][v]->tick();
//        float release = ref[EnvelopeRelease][v]->tick();
//        float leak = ref[EnvelopeLeak][v]->tick();
//
//        tADSRT_setAttack(&envs[v], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&envs[v], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&envs[v], sustain);
//        tADSRT_setRelease(&envs[v], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&envs[v], ((1.0f - leak) * 0.00005f) + 0.99995f);
        
        value[v] = tADSRT_tick(&envs[v]);
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

float* Envelope::getValuePointer()
{
    return value;
}
