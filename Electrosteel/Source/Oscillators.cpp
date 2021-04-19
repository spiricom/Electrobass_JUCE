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
                       AudioProcessorValueTreeState& vts, StringArray s) :
AudioComponent(n, p, vts, s)
{
}

void Oscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}

//==============================================================================

SawPulseOscillator::SawPulseOscillator(const String& n, ESAudioProcessor& p,
                                       AudioProcessorValueTreeState& vts, StringArray s) :
Oscillator(n, p, vts, s)
{
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
    Oscillator::prepareToPlay(sampleRate, samplesPerBlock);
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
    float transpose = params[v][SawPulseTranspose]->tick();
    float shape = params[v][SawPulseShape]->tick();
    float detuneAmount = params[v][SawPulseDetune]->tick();
    float volume = params[v][SawPulseVolume]->tick();
    
//    float tempLFO1 = (tCycle_tick(&pwmLFO1) * 0.25f) + 0.5f; // pulse length
//    float tempLFO2 = ((tCycle_tick(&pwmLFO2) * 0.25f) + 0.5f) * tempLFO1; // open length
    
    for (int i = 0; i < NUM_OSC_PER_VOICE; i++)
    {
        float tempFreq = processor.voiceFreq[v] * (1.0f + (detune[v][i] * detuneAmount));
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





