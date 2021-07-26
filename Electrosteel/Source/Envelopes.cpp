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
MappingSourceModel(p, n, true, false, Colours::deepskyblue)
{
    for (int i = 0; i < p.numInvParameterSkews; ++i)
    {
        sourceValues[i] = (float*) leaf_alloc(&p.leaf, sizeof(float) * NUM_STRINGS);
        sources[i] = &sourceValues[i];
    }
    
    useVelocity = vts.getParameter(n + " Velocity");
    
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
    }
}

Envelope::~Envelope()
{
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        leaf_free(&processor.leaf, (char*)sourceValues[i]);
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tADSRT_free(&envs[i]);
    }
}

void Envelope::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tADSRT_setSampleRate(&envs[i], sampleRate);
    }
}

void Envelope::frame()
{
    sampleInBlock = 0;
    // only enabled if it's actually being used as a source
    enabled = processor.sourceMappingCounts[getName()] > 0;
}

void Envelope::tick()
{
    if (!enabled) return;
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; v++)
    {
        float attack = quickParams[EnvelopeAttack][v]->tickNoSmoothing();
        float decay = quickParams[EnvelopeDecay][v]->tickNoSmoothing();
        float sustain = quickParams[EnvelopeSustain][v]->tickNoSmoothing();
        float release = quickParams[EnvelopeRelease][v]->tickNoSmoothing();
        float leak = quickParams[EnvelopeLeak][v]->tickNoSmoothing();
        attack = attack < 0.f ? 0.f : attack;
        decay = decay < 0.f ? 0.f : decay;
        sustain = sustain < 0.f ? 0.f : sustain;
        release = release < 0.f ? 0.f : release;
        leak = leak < 0.f ? 0.f : leak;
        
//        tADSRT_setAttack(&envs[v], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&envs[v], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&envs[v], sustain);
//        tADSRT_setRelease(&envs[v], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setAttack(&envs[v], attack);
        tADSRT_setDecay(&envs[v], decay);
        tADSRT_setSustain(&envs[v], sustain);
        tADSRT_setRelease(&envs[v], release);
        tADSRT_setLeakFactor(&envs[v], 0.99995f + 0.00005f*(1.f-leak));

        float value = tADSRT_tickNoInterp(&envs[v]);
        
        sourceValues[0][v] = value;
        for (int i = 1; i < processor.numInvParameterSkews; ++i)
        {
            float invSkew = processor.quickInvParameterSkews[i];
            sourceValues[i][v] = powf(value, invSkew);
        }
        
        if (processor.strings[0]->numVoices > 1)
        {
            if (processor.strings[0]->voices[v][0] == -2)
            {
                if (envs[v]->whichStage == env_idle)
                {
                    tSimplePoly_deactivateVoice(&processor.strings[0], v);
                    processor.voiceIsSounding[v] = false;
                }
            }
        }
    }
//    sampleInBlock++;
}

void Envelope::noteOn(int voice, float velocity)
{
    if (useVelocity->getValue() == 0) velocity = 1.f;
    tADSRT_on(&envs[voice], velocity);
    processor.voiceIsSounding[voice] = true;
}

void Envelope::noteOff(int voice, float velocity)
{
    tADSRT_off(&envs[voice]);
}
