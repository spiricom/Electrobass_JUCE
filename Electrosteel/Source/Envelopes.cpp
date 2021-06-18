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
                   AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cEnvelopeParams, false),
MappingSourceModel(p, n, &envValues[0], true, true, false, Colours::cyan)
{
    useVelocity = vts.getParameter(n + "Velocity");
    
    //exponential buffer rising from 0 to 1
    LEAF_generate_exp(expBuffer, 1000.0f, -1.0f, 0.0f, -0.0008f, EXP_BUFFER_SIZE);
    
    // exponential decay buffer falling from 1 to
    LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE);
    
    expBufferSizeMinusOne = EXP_BUFFER_SIZE - 1;
    decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tADSRT_init(&envs[i], expBuffer[0] * 8192.0f,
                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &processor.leaf);
        tADSRT_setLeakFactor(&envs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
        envValues[i] = (float*)leaf_calloc(&processor.leaf, sizeof(float) * currentBlockSize);
    }
}

Envelope::~Envelope()
{
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        leaf_free(&processor.leaf, (char*)envValues[i]);
    }
}

void Envelope::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tADSRT_setSampleRate(&envs[i], sampleRate);
        leaf_free(&processor.leaf, (char*)envValues[i]);
    }
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        envValues[i] = (float*)leaf_calloc(&processor.leaf, sizeof(float) * currentBlockSize);
    }
}

void Envelope::frame()
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

void Envelope::tick()
{
    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < NUM_STRINGS; v++)
    {
        float attack = values[EnvelopeAttack][v]*a + lastValues[EnvelopeAttack][v]*(1.f-a);
        float decay = values[EnvelopeDecay][v]*a + lastValues[EnvelopeDecay][v]*(1.f-a);
        float sustain = values[EnvelopeSustain][v]*a + lastValues[EnvelopeSustain][v]*(1.f-a);
        float release = values[EnvelopeRelease][v]*a + lastValues[EnvelopeRelease][v]*(1.f-a);
        float leak = values[EnvelopeLeak][v]*a + lastValues[EnvelopeLeak][v]*(1.f-a);
        attack = attack < 0.f ? 0.f : attack;
        decay = decay < 0.f ? 0.f : decay;
        sustain = sustain < 0.f ? 0.f : sustain;
        release = release < 0.f ? 0.f : release;
        leak = leak < 0.f ? 0.f : leak;
        
        tADSRT_setAttack(&envs[v], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setDecay(&envs[v], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setSustain(&envs[v], sustain);
        tADSRT_setRelease(&envs[v], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setLeakFactor(&envs[v], 0.99995f*leak + (1.f-leak));
        envValues[v][sampleInBlock] = tADSRT_tickNoInterp(&envs[v]);
    }
    sampleInBlock++;
}

void Envelope::noteOn(int voice, float velocity)
{
    if (useVelocity->getValue() == 0) velocity = 1.f;
    tADSRT_on(&envs[voice], velocity);
}

void Envelope::noteOff(int voice, float velocity)
{
    tADSRT_off(&envs[voice]);
}
