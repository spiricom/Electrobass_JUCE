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

Oscillator::Oscillator(const String& n, ESAudioProcessor& p,
                       AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cOscParams, true)
{
    for (int i = 0; i < OscParamNil; ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tSawtooth_init(&saw[i], &processor.leaf);
        tRosenbergGlottalPulse_init(&pulse[i], &processor.leaf);
        tRosenbergGlottalPulse_setOpenLength(&pulse[i], 0.3f);
        tRosenbergGlottalPulse_setPulseLength(&pulse[i], 0.4f);
    }
    
    afpFilterSend = vts.getRawParameterValue(n + "FilterSend");
}

Oscillator::~Oscillator()
{
    
}

void Oscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tSawtooth_setSampleRate(&saw[i], sampleRate);
        tRosenbergGlottalPulse_setSampleRate(&pulse[i], sampleRate);
    }
//    tCycle_setSampleRate(&pwmLFO1, sampleRate);
//    tCycle_setSampleRate(&pwmLFO2, sampleRate);
}

void Oscillator::frame()
{
    enabled = *afpEnabled > 0;
}

void Oscillator::tick(int v, float* output)
{
    if (!enabled) return;
        
    float pitch = ref[OscPitch][v]->tick();
    float fine = ref[OscFine][v]->tick()*0.01f;
    float shape = ref[OscShape][v]->tick();
    float volume = ref[OscAmp][v]->tick();
    
//    float tempLFO1 = (tCycle_tick(&pwmLFO1) * 0.25f) + 0.5f; // pulse length
//    float tempLFO2 = ((tCycle_tick(&pwmLFO2) * 0.25f) + 0.5f) * tempLFO1; // open length
    
    float note = processor.voiceNote[v];
    float freq = mtof(note + pitch + fine);
    tSawtooth_setFreq(&saw[v], freq);
    tRosenbergGlottalPulse_setFreq(&pulse[v], freq);
//        tRosenbergGlottalPulse_setPulseLength(&glottal[v][i], tempLFO1);
//        tRosenbergGlottalPulse_setOpenLength(&glottal[v][i], tempLFO2);
    
    float sample = 0.0f;
    
    sample += tSawtooth_tick(&saw[v]) * (1.0f - shape);
    sample += tRosenbergGlottalPulse_tickHQ(&pulse[v]) * shape;
    
    sample *= volume;
    
    float f = *afpFilterSend;
    output[0] += sample*f;
    output[1] += sample*(1.f-f);
}


//==============================================================================

LowFreqOscillator::LowFreqOscillator(const String& n, ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cLowFreqParams, false)
{
    for (int i = 0; i < LowFreqParamNil; ++i)
    {
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            ref[i][v] = params[i]->getUnchecked(v);
        }
    }
    
    sync = vts.getParameter(n + "Sync");
    
    for (int i = 0; i < NUM_STRINGS; i++)
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
    for (int i = 0; i < NUM_STRINGS; i++)
    {
        tCycle_setSampleRate(&lfo[i], sampleRate);
    }
}

void LowFreqOscillator::frame()
{
    for (int v = 0; v < NUM_STRINGS; v++)
    {
        phaseReset = ref[LowFreqSyncPhase][v]->skip(currentSamplesPerBlock);
    }
}

void LowFreqOscillator::tick()
{
    for (int v = 0; v < NUM_STRINGS; v++)
    {
        float rate = ref[LowFreqRate][v]->tick();
        
        tCycle_setFreq(&lfo[v], rate);
        
        value[v] = tCycle_tick(&lfo[v]);
    }
}

void LowFreqOscillator::noteOn(int voice, float velocity)
{
    //TODO: gotta put phase sync back into LEAF
    if (sync->getValue() > 0) tCycle_setPhase(&lfo[voice], phaseReset);
}

void LowFreqOscillator::noteOff(int voice, float velocity)
{
    
}

float* LowFreqOscillator::getValuePointer()
{
    return value;
}
