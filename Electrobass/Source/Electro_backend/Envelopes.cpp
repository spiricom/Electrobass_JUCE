/*
  ==============================================================================

    Envelopes.cpp
    Created: 17 Mar 2021 4:17:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Envelopes.h"
#include "../PluginProcessor.h"

//==============================================================================
Envelope::Envelope(const String& n, ElectroAudioProcessor& p,
                   AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cEnvelopeParams, false),
MappingSourceModel(p, n, true, false, Colours::deepskyblue)
{
    setParams();
    useVelocity = vts.getParameter(n + " Velocity");
    
    //exponential buffer rising from 0 to 1
    LEAF_generate_exp(expBuffer, 1000.0f, -1.0f, 0.0f, -0.0008f, EXP_BUFFER_SIZE);
    
    // exponential decay buffer falling from 1 to
    LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE);
    
    expBufferSizeMinusOne = EXP_BUFFER_SIZE - 1;
    decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
    
    for (int i = 0; i < MAX_NUM_VOICES; i++)
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
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tADSRT_free(&envs[i]);
    }
}

void Envelope::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < MAX_NUM_VOICES; i++)
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


void Envelope::loadAll(int v)
{
    quickParams[EnvelopeAttack][v]->setValueToRaw();
    quickParams[EnvelopeDecay][v]->setValueToRaw();
    quickParams[EnvelopeSustain][v]->setValueToRaw();
    quickParams[EnvelopeRelease][v]->setValueToRaw();
    quickParams[EnvelopeLeak][v]->setValueToRaw();
    float attack = quickParams[EnvelopeAttack][v]->read();
    attack = attack < 0.f ? 0.f : attack;
    tADSRT_setAttack(&envs[v], attack);
    float decay = quickParams[EnvelopeDecay][v]->read();
    decay = decay < 0.f ? 0.f : decay;
    tADSRT_setDecay(&envs[v], decay);
    float sustain = quickParams[EnvelopeSustain][v]->read();
    sustain = sustain < 0.f ? 0.f : sustain;
    tADSRT_setSustain(&envs[v], sustain);
    float release = quickParams[EnvelopeRelease][v]->read();
    release = release < 0.f ? 0.f : release;
    tADSRT_setRelease(&envs[v], release);
    float leak = quickParams[EnvelopeLeak][v]->read();
    leak = leak < 0.f ? 0.f : leak;
    tADSRT_setLeakFactor(&envs[v], 0.99995f + 0.00005f*(1.f-leak));
}

void Envelope::tick()
{
    if (!enabled) return;
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; v++)
    {
#if 0
        if(processor.knobsToSmooth.contains(quickParams[EnvelopeAttack][v]))
        {
            float attack = quickParams[EnvelopeAttack][v]->read();
            attack = attack < 0.f ? 0.f : attack;
            tADSRT_setAttack(&envs[v], attack);
        }
        if(processor.knobsToSmooth.contains(quickParams[EnvelopeDecay][v]))
        {
            float decay = quickParams[EnvelopeDecay][v]->read();
            decay = decay < 0.f ? 0.f : decay;
            tADSRT_setDecay(&envs[v], decay);
        }
        if(processor.knobsToSmooth.contains(quickParams[EnvelopeSustain][v]))
        {
            float sustain = quickParams[EnvelopeSustain][v]->read();
            sustain = sustain < 0.f ? 0.f : sustain;
            tADSRT_setSustain(&envs[v], sustain);
        }
        if(processor.knobsToSmooth.contains(quickParams[EnvelopeRelease][v]))
        {
            float release = quickParams[EnvelopeRelease][v]->read();
            release = release < 0.f ? 0.f : release;
            tADSRT_setRelease(&envs[v], release);
        }
        if(processor.knobsToSmooth.contains(quickParams[EnvelopeLeak][v]))
        {
            float leak = quickParams[EnvelopeLeak][v]->read();
            leak = leak < 0.f ? 0.f : leak;
            tADSRT_setLeakFactor(&envs[v], 0.99995f + 0.00005f*(1.f-leak));
        }
        
#endif
#if 1
        float attack = quickParams[EnvelopeAttack][v]->read();
        attack = attack < 0.f ? 0.f : attack;
        attack = processor.scaleADSRTimes(attack);
        tADSRT_setAttack(&envs[v], attack);
        float decay = quickParams[EnvelopeDecay][v]->read();
        decay = decay < 0.f ? 0.f : decay;
        decay = processor.scaleADSRTimes(decay);
        tADSRT_setDecay(&envs[v], decay);
        float sustain = quickParams[EnvelopeSustain][v]->read();
        sustain = sustain < 0.f ? 0.f : sustain;
        tADSRT_setSustain(&envs[v], sustain);
        float release = quickParams[EnvelopeRelease][v]->read();
        release = release < 0.f ? 0.f : release;
        release = processor.scaleADSRTimes(release);
        tADSRT_setRelease(&envs[v], release);
        float leak = quickParams[EnvelopeLeak][v]->read();
        leak = leak < 0.f ? 0.f : leak;
        tADSRT_setLeakFactor(&envs[v], 0.99995f + 0.00005f*(1.f-leak));
#endif
        float value = tADSRT_tick(&envs[v]);
        
        source[v] = value;
        if (isAmpEnv)
        {
                if (processor.strings[0]->voices[v][0] == -2)
                {
                    if (envs[v]->whichStage == env_idle)
                    {
                        tSimplePoly_deactivateVoice(&processor.strings[0], (uint8_t) v);
                        processor.voiceIsSounding[v] = false;
                    }
                }
        }
    }
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

void Envelope::setParams()
{
    //        "Attack",
    //        "Decay",
    //        "Sustain",
    //        "Release",
    //        "Leak"
    for (int i = 0; i < 2; ++i)
    {
        String pn = AudioComponent::name + " " + paramNames[i];
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < MAX_NUM_VOICES; ++v)
        {
            
            params[i]->add(new SmoothedParameter(processor, vts, pn));// 0.0f, 20000.0f, 4000.0f));
            quickParams[i][v] = params[i]->getUnchecked(v);
        }
        for (int t = 0; t < 3; ++t)
        {
            String targetName = pn + " T" + String(t+1);
            targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
            processor.addMappingTarget(targets.getLast());
        }
    }
    //sustain
    String pn = AudioComponent::name + " " + paramNames[2];
    params.add(new OwnedArray<SmoothedParameter>());
    for (int v = 0; v < MAX_NUM_VOICES; ++v)
    {
       
        params[2]->add(new SmoothedParameter(processor, vts, pn));
        quickParams[2][v] = params[2]->getUnchecked(v);
    }
    for (int t = 0; t < 3; ++t)
    {
        String targetName = pn + " T" + String(t+1);
        targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
        processor.addMappingTarget(targets.getLast());
    }
    //release
    pn = AudioComponent::name + " " + paramNames[3];
    params.add(new OwnedArray<SmoothedParameter>());
    for (int v = 0; v < MAX_NUM_VOICES; ++v)
    {
        
        params[3]->add(new SmoothedParameter(processor, vts, pn));// 0.0f, 20000.0f, 4000.0f));
        quickParams[3][v] = params[3]->getUnchecked(v);
    }
    for (int t = 0; t < 3; ++t)
    {
        String targetName = pn + " T" + String(t+1);
        targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
        processor.addMappingTarget(targets.getLast());
    }
    // leak
    pn = AudioComponent::name + " " + paramNames[4];
    params.add(new OwnedArray<SmoothedParameter>());
    for (int v = 0; v < MAX_NUM_VOICES; ++v)
    {
       
        params[4]->add(new SmoothedParameter(processor, vts, pn));
        quickParams[4][v] = params[4]->getUnchecked(v);
    }
    for (int t = 0; t < 3; ++t)
    {
        String targetName = pn + " T" + String(t+1);
        targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
        processor.addMappingTarget(targets.getLast());
    }
     
}
