///*
//  ==============================================================================
//
//    Synths.cpp
//    Created: 15 Mar 2021 11:29:46am
//    Author:  Matthew Wang
//
//  ==============================================================================
//*/
//
//#include "Synths.h"
//#include "PluginProcessor.h"
//
////==============================================================================
////==============================================================================
//SubtractiveSynth::SubtractiveSynth(ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
//processor(p),
//vts(vts)
//{
//    for (int i = 0; i < SubtractiveKnobParamNil; ++i)
//    {
//        SmoothedParameter p (vts.getRawParameterValue(cSubtractiveKnobParamNames[i]));
//        params.add(p);
//    }
//
//    LEAF& leaf = shared.leaf;
//    float* detune = shared.detune;
//
//
//    //exponential buffer rising from 0 to 1
//    LEAF_generate_exp(expBuffer, 1000.0f, -1.0f, 0.0f, -0.0008f, EXP_BUFFER_SIZE);
//
//    // exponential decay buffer falling from 1 to
//    LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE);
//
//    expBufferSizeMinusOne = EXP_BUFFER_SIZE - 1;
//    decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
//
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        tEfficientSVF_init(&synthLP[i], SVFTypeLowpass, 2000, 0.4f, &leaf);
//        tADSRT_init(&polyEnvs[i], expBuffer[0] * 8192.0f,
//                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
//                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
//                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
//                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
//        tADSRT_setLeakFactor(&polyEnvs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
//
//        tADSRT_init(&polyFiltEnvs[i], expBuffer[0] * 8192.0f,
//                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
//                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
//                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
//                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
//        tADSRT_setLeakFactor(&polyFiltEnvs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
//    }

//}
//
//SubtractiveSynth::~SubtractiveSynth()
//{
//}
//
//void SubtractiveSynth::prepareToPlay (double sampleRate, int samplesPerBlock)
//{
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
//        {
//            int k = (i * NUM_OSC_PER_VOICE) + j;
//            tSawtooth_setSampleRate(&osc[k], sampleRate);
//            tRosenbergGlottalPulse_setSampleRate(&glottal[k], sampleRate);
//        }
////        tADSRT_setSampleRate(&polyEnvs[i], sampleRate);
////        tADSRT_setSampleRate(&polyFiltEnvs[i], sampleRate);
//    }
//    tCycle_setSampleRate(&pwmLFO1, sampleRate);
//    tCycle_setSampleRate(&pwmLFO2, sampleRate);
//}
//
//void SubtractiveSynth::frame()
//{
//    float* expBuffer = shared.expBuffer;
//    float expBufferSizeMinusOne = shared.expBufferSizeMinusOne;
//
//    float q = params[SubtractiveFilterQ];
//    float attack = params[SubtractiveAttack];
//    float decay = params[SubtractiveDecay];
//    float sustain = params[SubtractiveSustain];
//    float release = params[SubtractiveRelease];
//    float leak = params[SubtractiveLeak];
//    float filtAttack = params[SubtractiveFilterAttack];
//    float filtDecay = params[SubtractiveFilterDecay];
//    float filtSustain = params[SubtractiveFilterSustain];
//    float filtRelease = params[SubtractiveFilterRelease];
//    float filtLeak = params[SubtractiveFilterLeak];
//
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        tEfficientSVF_setQ(&shared.synthLP[i], (q * 2.0f) + 0.4f);
//        tADSRT_setAttack(&shared.polyEnvs[i], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyEnvs[i], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyEnvs[i], sustain);
//        tADSRT_setRelease(&shared.polyEnvs[i], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyEnvs[i], ((1.0f - leak) * 0.00005f) + 0.99995f);
//
//        tADSRT_setAttack(&shared.polyFiltEnvs[i], expBuffer[(int)(filtAttack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyFiltEnvs[i], expBuffer[(int)(filtDecay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyFiltEnvs[i], filtSustain);
//        tADSRT_setRelease(&shared.polyFiltEnvs[i], expBuffer[(int)(filtRelease * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyFiltEnvs[i], ((1.0f - filtLeak) * 0.00005f) + 0.99995f);
//    }
//}
//
//float SubtractiveSynth::tick()
//{
//    float sample = 0.0f;

//float q = params[SubtractiveFilterQ];
//    float attack = params[SubtractiveAttack];
//    float decay = params[SubtractiveDecay];
//    float sustain = params[SubtractiveSustain];
//    float release = params[SubtractiveRelease];
//    float leak = params[SubtractiveLeak];
//    float filtAttack = params[SubtractiveFilterAttack];
//    float filtDecay = params[SubtractiveFilterDecay];
//    float filtSustain = params[SubtractiveFilterSustain];
//    float filtRelease = params[SubtractiveFilterRelease];
//    float filtLeak = params[SubtractiveFilterLeak];
//
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        tEfficientSVF_setQ(&shared.synthLP[i], (q * 2.0f) + 0.4f);
//        tADSRT_setAttack(&shared.polyEnvs[i], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyEnvs[i], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyEnvs[i], sustain);
//        tADSRT_setRelease(&shared.polyEnvs[i], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyEnvs[i], ((1.0f - leak) * 0.00005f) + 0.99995f);
//
//        tADSRT_setAttack(&shared.polyFiltEnvs[i], expBuffer[(int)(filtAttack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyFiltEnvs[i], expBuffer[(int)(filtDecay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyFiltEnvs[i], filtSustain);
//        tADSRT_setRelease(&shared.polyFiltEnvs[i], expBuffer[(int)(filtRelease * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyFiltEnvs[i], ((1.0f - filtLeak) * 0.00005f) + 0.99995f);
//    }
//
//    tSimplePoly* voice = shared.voice;
//    float* freq = shared.freq;
//    float* detune = shared.detune;
//
//    float volume = params[SubtractiveVolume];
//    float detuneAmount = params[SubtractiveDetuneAmount];
//    float shape = params[SubtractiveShape];
//    float keyFollow = params[SubtractiveFilterKeyFollow];
//    float cutoff = params[SubtractiveFilterCutoff];
//    float amount = params[SubtractiveFilterAmount];
//
//    //==============================================================================
//    float tempLFO1 = (tCycle_tick(&pwmLFO1) * 0.25f) + 0.5f; // pulse length
//    float tempLFO2 = ((tCycle_tick(&pwmLFO2) * 0.25f) + 0.5f) * tempLFO1; // open length
//
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        float midiNote = tSimplePoly_getPitch(&voice[i], 0);
//
//        shared.calcVoiceFreq(i);
//
//        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
//        {
//            int k = (i * NUM_OSC_PER_VOICE) + j;
//            float tempFreq = freq[i] * (1.0f + (detune[k] * detuneAmount));
//            tSawtooth_setFreq(&osc[k], tempFreq);
//            tRosenbergGlottalPulse_setFreq(&glottal[k], tempFreq);
//            tRosenbergGlottalPulse_setPulseLength(&glottal[k], tempLFO1);
//            tRosenbergGlottalPulse_setOpenLength(&glottal[k], tempLFO2);
//        }
//
//        float keyFollowFilt = midiNote * keyFollow * 64.0f;
//        float tempFreq2 = cutoff * 4096.0f + keyFollowFilt;
//        tempFreq2 = LEAF_clip(0.0f, tempFreq2, 4095.0f);
//        shared.filtFreqs[i] = (uint16_t) tempFreq2;
//
//        float tempSample = 0.0f;
//        float env = tADSRT_tick(&shared.polyEnvs[i]);
//
//        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
//        {
//            int k = (i * NUM_OSC_PER_VOICE) + j;
//            tempSample += tSawtooth_tick(&osc[k]) * env * (1.0f - shape);
//            tempSample += tRosenbergGlottalPulse_tickHQ(&glottal[k]) * env * shape;
//        }
//        float f = (amount * 4095.0f * tADSRT_tick(&shared.polyFiltEnvs[i])) + shared.filtFreqs[i];
//        tEfficientSVF_setFreq(&shared.synthLP[i], LEAF_clip(0.0f, f, 4095.0f));
//        sample += tEfficientSVF_tick(&shared.synthLP[i], tempSample);
//    }
//
//    sample *= INV_NUM_OSC_PER_VOICE * volume;
//    sample = tanhf(sample);
//    return sample * shared.ccValues[0];
//}
//
//void SubtractiveSynth::noteOn(int voice, float velocity)
//{
//    tADSRT_on(&shared.polyEnvs[voice], velocity);
//    tADSRT_on(&shared.polyFiltEnvs[voice], velocity);
//}
//
//void SubtractiveSynth::noteOff(int voice, float velocity)
//{
//    tADSRT_off(&shared.polyEnvs[voice]);
//    tADSRT_off(&shared.polyFiltEnvs[voice]);
//}
//
////==============================================================================
////==============================================================================
//WavetableSynth::WavetableSynth(ESAudioProcessor& p, AudioProcessorValueTreeState& vts,
//                               SharedSynthResources& ssr) :
//processor(p),
//vts(vts),
//shared(ssr)
//{
//    for (int i = 0; i < WavetableKnobParamNil; ++i)
//    {
//        SmoothedParameter p (vts.getRawParameterValue(cWavetableKnobParamNames[i]));
//        params.add(p);
//    }
//
//    int sizes[1];
//    sizes[0] = wavetable.getNumSamples();
//    tWaveSynth_init(&waveSynth, NUM_VOICES, (float**)wavetable.getArrayOfReadPointers(), sizes, 1, 20000.0f, &shared.leaf);
//}
//
//WavetableSynth::~WavetableSynth()
//{
//}
//
//void WavetableSynth::prepareToPlay (double sampleRate, int samplesPerBlock)
//{
//    tWaveSynth_setSampleRate(&waveSynth, sampleRate);
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//
//    }
//}
//
//void WavetableSynth::frame()
//{
//    float* expBuffer = shared.expBuffer;
//    float expBufferSizeMinusOne = shared.expBufferSizeMinusOne;
//
//    float q = params[WavetableFilterQ];
//    float attack = params[WavetableAttack];
//    float decay = params[WavetableDecay];
//    float sustain = params[WavetableSustain];
//    float release = params[WavetableRelease];
//    float leak = params[WavetableLeak];
//    float filtAttack = params[WavetableFilterAttack];
//    float filtDecay = params[WavetableFilterDecay];
//    float filtSustain = params[WavetableFilterSustain];
//    float filtRelease = params[WavetableFilterRelease];
//    float filtLeak = params[WavetableFilterLeak];
//
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        tEfficientSVF_setQ(&shared.synthLP[i], (q * 2.0f) + 0.4f);
//        tADSRT_setAttack(&shared.polyEnvs[i], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyEnvs[i], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyEnvs[i], sustain);
//        tADSRT_setRelease(&shared.polyEnvs[i], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyEnvs[i], ((1.0f - leak) * 0.00005f) + 0.99995f);
//
//        tADSRT_setAttack(&shared.polyFiltEnvs[i], expBuffer[(int)(filtAttack * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setDecay(&shared.polyFiltEnvs[i], expBuffer[(int)(filtDecay * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setSustain(&shared.polyFiltEnvs[i], filtSustain);
//        tADSRT_setRelease(&shared.polyFiltEnvs[i], expBuffer[(int)(filtRelease * expBufferSizeMinusOne)] * 8192.0f);
//        tADSRT_setLeakFactor(&shared.polyFiltEnvs[i], ((1.0f - filtLeak) * 0.00005f) + 0.99995f);
//    }
//}
//
//float WavetableSynth::tick()
//{
//    float sample = 0.0f;
//
//    tSimplePoly* voice = shared.voice;
//
//    float volume = params[WavetableVolume];
//    float phase = params[WavetablePhase];
//    float keyFollow = params[WavetableFilterKeyFollow];
//    float cutoff = params[WavetableFilterCutoff];
//    float amount = params[WavetableFilterAmount];
//
//    //==============================================================================
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        float midiNote = tSimplePoly_getPitch(&voice[i], 0);
//
//        shared.calcVoiceFreq(i);
//
//        tWaveSynth_setIndex(&waveSynth, 0.);
//        for (int i = 0; i < 1; ++i)
//        {
//            tWaveSynth_setIndexGain(&waveSynth, i, volume);
//            tWaveSynth_setIndexPhase(&waveSynth, i, phase);
//        }
//        tWaveSynth_setFreq(&waveSynth, i, shared.freq[i]);
//
//        float keyFollowFilt = midiNote * keyFollow * 64.0f;
//        float tempFreq2 = cutoff * 4096.0f + keyFollowFilt;
//        tempFreq2 = LEAF_clip(0.0f, tempFreq2, 4095.0f);
//        shared.filtFreqs[i] = (uint16_t) tempFreq2;
//
//        float env = tADSRT_tick(&shared.polyEnvs[i]);
//
//        float tempSample = tWaveSynth_tickVoice(&waveSynth, i) * env;
//
//        float f = (amount * 4095.0f * tADSRT_tick(&shared.polyFiltEnvs[i])) + shared.filtFreqs[i];
//        tEfficientSVF_setFreq(&shared.synthLP[i], LEAF_clip(0.0f, f, 4095.0f));
//        sample += tEfficientSVF_tick(&shared.synthLP[i], tempSample);
//    }
//
//    sample = tanhf(sample);
//    return sample * shared.ccValues[0];
//}
//
//void WavetableSynth::noteOn(int voice, float velocity)
//{
//    tADSRT_on(&shared.polyEnvs[voice], velocity);
//    tADSRT_on(&shared.polyFiltEnvs[voice], velocity);
//}
//
//void WavetableSynth::noteOff(int voice, float velocity)
//{
//    tADSRT_off(&shared.polyEnvs[voice]);
//    tADSRT_off(&shared.polyFiltEnvs[voice]);
//}
