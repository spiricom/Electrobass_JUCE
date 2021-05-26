/*
  ==============================================================================

    Oscillators.cpp
    Created: 17 Mar 2021 2:49:31pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Oscillators.h"
#include "PluginProcessor.h"

//==============================================================================

SawPulseOscillator::SawPulseOscillator(const String& n, ESAudioProcessor& p,
                                       AudioProcessorValueTreeState& vts, StringArray s) :
AudioComponent(n, p, vts, s)
{
    for (int i = 0; i < SawPulseParamNil; ++i)
    {
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
        {
            tSawtooth_init(&saw[i][j], &processor.leaf);
            tRosenbergGlottalPulse_init(&pulse[i][j], &processor.leaf);
            tRosenbergGlottalPulse_setOpenLength(&pulse[i][j], 0.3f);
            tRosenbergGlottalPulse_setPulseLength(&pulse[i][j], 0.4f);
            detune[i][j] = ((processor.leaf.random() * 0.0264f) - 0.0132f);
        }
    }
//    tCycle_init(&pwmLFO1, &processor.leaf);
//    tCycle_init(&pwmLFO2, &processor.leaf);
//    tCycle_setFreq(&pwmLFO1, 0.63f);
//    tCycle_setFreq(&pwmLFO2, 0.7211f);
}

SawPulseOscillator::~SawPulseOscillator()
{
    
}

void SawPulseOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_VOICES; i++)
    {
        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
        {
            tSawtooth_setSampleRate(&saw[i][j], sampleRate);
            tRosenbergGlottalPulse_setSampleRate(&pulse[i][j], sampleRate);
        }
    }
//    tCycle_setSampleRate(&pwmLFO1, sampleRate);
//    tCycle_setSampleRate(&pwmLFO2, sampleRate);
}

//void SawPulseOscillator::frame(int v)
//{
//
//}

float SawPulseOscillator::tick(int v)
{
    float pitch = ref[SawPulsePitch][v]->tick();
    float fine = ref[SawPulseFine][v]->tick();
    float shape = ref[SawPulseShape][v]->tick();
    float detuneAmount = ref[SawPulseDetune][v]->tick();
    float volume = ref[SawPulseVolume][v]->tick();
    
//    float tempLFO1 = (tCycle_tick(&pwmLFO1) * 0.25f) + 0.5f; // pulse length
//    float tempLFO2 = ((tCycle_tick(&pwmLFO2) * 0.25f) + 0.5f) * tempLFO1; // open length
    
    for (int i = 0; i < NUM_OSC_PER_VOICE; i++)
    {
        float tempFreq = processor.voiceFreq[v] * (1.0f + (detune[v][i] * detuneAmount));
        tempFreq = mtof((ftom(tempFreq) + pitch + fine*0.01f));
        tSawtooth_setFreq(&saw[v][i], tempFreq);
        tRosenbergGlottalPulse_setFreq(&pulse[v][i], tempFreq);
//        tRosenbergGlottalPulse_setPulseLength(&glottal[v][i], tempLFO1);
//        tRosenbergGlottalPulse_setOpenLength(&glottal[v][i], tempLFO2);
    }
    
    float sample = 0.0f;
    
    for (int i = 0; i < NUM_OSC_PER_VOICE; i++)
    {
        sample += tSawtooth_tick(&saw[v][i]) * (1.0f - shape);
        sample += tRosenbergGlottalPulse_tickHQ(&pulse[v][i]) * shape;
    }
    
    sample *= INV_NUM_OSC_PER_VOICE * volume;
    sample = tanhf(sample);
    return sample;
}


//==============================================================================

LowFreqOscillator::LowFreqOscillator(const String& n, ESAudioProcessor& p, AudioProcessorValueTreeState& vts, StringArray s) :
AudioComponent(n, p, vts, s)
{
    for (int i = 0; i < LowFreqParamNil; ++i)
    {
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    sync = vts.getParameter(n + "Sync");
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tCycle_init(&lfo[i], &processor.leaf);
        value[i] = 0.0f;
    }
    
    phaseReset = 0.0f;
}

LowFreqOscillator::~LowFreqOscillator()
{
    
}

void LowFreqOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tCycle_setSampleRate(&lfo[i], sampleRate);
    }
}

void LowFreqOscillator::tick()
{
    for (int v = 0; v < NUM_VOICES; v++)
    {
        float rate = ref[LowFreqRate][v]->tick();
        phaseReset = ref[LowFreqSyncPhase][v]->tick();
        
        tCycle_setFreq(&lfo[v], rate);
        
        value[v] = tCycle_tick(&lfo[v]);
    }
}

void LowFreqOscillator::noteOn(int voice, float velocity)
{
    if (sync->getValue() > 0) tCycle_setPhase(&lfo[voice], phaseReset);
}

void LowFreqOscillator::noteOff(int voice, float velocity)
{
    
}

float* LowFreqOscillator::getValuePointer()
{
    return value;
}
