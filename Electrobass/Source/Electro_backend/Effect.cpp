/*
  ==============================================================================

    Effect.cpp
    Created: 25 May 2022 2:45:37pm
    Author:  Davis Polito

  ==============================================================================
*/

#include "Effect.h"
#include "../PluginProcessor.h"
Effect::Effect(const String& n, ElectroAudioProcessor& p,
               AudioProcessorValueTreeState& vts) : AudioComponent(n, p, vts, cFXParams, false)
{
    setParams();
    _tick = typeToTick(Softclip);
    //int temp = processor.leaf.clearOnAllocation;
    getProcessor()->leaf.clearOnAllocation = 1;

    
    //getProcessor()->leaf.clearOnAllocation = temp;
    afpFXType = vts.getRawParameterValue(n + " FXType");
    inv_oversample = 1.0f / OVERSAMPLE;
    for (int i = 0; i < MAX_NUM_VOICES; i++){
        tCrusher_init(&bc[i],&getProcessor()->leaf);
        tHighpass_init(&dcBlock1[i], 5.0f,&getProcessor()->leaf);
        tHighpass_init(&dcBlock2[i], 5.0f,&getProcessor()->leaf);
        tVZFilter_init(&shelf1[i], Lowshelf, 80.0f, 6.0f,  &getProcessor()->leaf);
        tVZFilter_init(&shelf2[i], Highshelf, 12000.0f, 6.0f, &getProcessor()->leaf);
        tVZFilter_init(&bell1[i], Bell, 1000.0f, 1.9f,  &getProcessor()->leaf);
        tVZFilter_setSampleRate(&shelf1[i], getProcessor()->leaf.sampleRate * OVERSAMPLE);
        tVZFilter_setSampleRate(&shelf2[i], getProcessor()->leaf.sampleRate * OVERSAMPLE);
        tVZFilter_setSampleRate(&bell1[i], getProcessor()->leaf.sampleRate * OVERSAMPLE);
        tCompressor_init(&comp[i], &getProcessor()->leaf);
        tHermiteDelay_init(&delay1[i], 4000.0f, 4096, &getProcessor()->leaf);
        tHermiteDelay_init(&delay2[i], 4000.0f, 4096, &getProcessor()->leaf);
        tCycle_init(&mod1[i], &getProcessor()->leaf);
        tCycle_init(&mod2[i], &getProcessor()->leaf);
        tCycle_setFreq(&mod1[i], 0.2f);
        tCycle_setFreq(&mod2[i], 0.22222222222f);
        tTapeDelay_init(&tapeDelay[i], 2000, 30000, &getProcessor()->leaf);
        tSVF_init(&delayLP[i], SVFTypeLowpass, 16000.f, .7f, &getProcessor()->leaf);
        tSVF_init(&delayHP[i], SVFTypeHighpass, 20.f, .7f, &getProcessor()->leaf);
        tFeedbackLeveler_init(&feedbackControl[i], .99f, 0.01f, 0.125f, 0, &getProcessor()->leaf);
        
        //filters
        tSVF_init(&lowpass[i], SVFTypeLowpass, 2000.f, 0.7f, &processor.leaf);
        tSVF_init(&highpass[i], SVFTypeHighpass, 2000.f, 0.7f, &processor.leaf);
        tSVF_init(&bandpass[i], SVFTypeBandpass, 2000.f, 0.7f, &processor.leaf);
        tDiodeFilter_init(&diodeFilters[i], 2000.f, 1.0f, &processor.leaf);
        tVZFilter_init(&VZfilterPeak[i], Bell, 2000.f, 1.0f, &processor.leaf);
        tVZFilter_init(&VZfilterLS[i], Lowshelf, 2000.f, 1.0f, &processor.leaf);
        tVZFilter_init(&VZfilterHS[i], Highshelf, 2000.f, 1.0f, &processor.leaf);
        tVZFilter_init(&VZfilterBR[i], BandReject, 2000.f, 1.0f, &processor.leaf);
        tLadderFilter_init(&Ladderfilter[i], 2000.f, 1.0f, &processor.leaf);
    }
    LEAF_generate_table_skew_non_sym(resTable, 0.01f, 10.0f, 0.5f, SCALE_TABLE_SIZE);
}

Effect::~Effect()
{
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tCrusher_free(&bc[i]);
        tHighpass_free(&dcBlock1[i]);
        tHighpass_free(&dcBlock2[i]);
        tVZFilter_free(&bell1[i]);
        tVZFilter_free(&shelf1[i]);
        tVZFilter_free(&shelf2[i]);
        tCompressor_free(&comp[i]);
        tHermiteDelay_free(&delay1[i]);
        tHermiteDelay_free(&delay2[i]);
        tCycle_free(&mod1[i]);
        tCycle_free(&mod2[i]);
        
        //filters
        tSVF_free(&lowpass[i]);
        tSVF_free(&highpass[i]);
        tSVF_free(&bandpass[i]);
        tDiodeFilter_free(&diodeFilters[i]);
        tVZFilter_free(&VZfilterPeak[i]);
        tVZFilter_free(&VZfilterLS[i]);
        tVZFilter_free(&VZfilterHS[i]);
        tVZFilter_free(&VZfilterBR[i]);
        tLadderFilter_free(&Ladderfilter[i]);
    }
}

//void Effect::setTick(FXType t)
//{
//    _tick = typeToTick(t);
//}





void Effect::frame()
{
    sampleInBlock = 0;
    
    _tick = typeToTick(FXType(int(*afpFXType)));
}

Effect::EffectTick Effect::typeToTick(FXType type)
{
    switch(type)
    {
        case None:
            return &Effect::tick;
            break;
        case Softclip:
            return &Effect::softClipTick;
        case Hardclip:
            return &Effect::hardClipTick;
        case ABSaturator:
            return &Effect::satTick;
        break;
        case Tanh:
            return &Effect::tanhTick;
        break;
        case Shaper:
            return &Effect::shaperTick;
        break;
        case Compressor:
            return &Effect::compressorTick;
        break;
        case Chorus:
            return &Effect::chorusTick;
        break;
        case Bitcrush:
            return &Effect::bcTick;
        break;
        case TiltFilter:
            return &Effect::tiltFilterTick;
        break;
        case Wavefolder:
            return &Effect::wavefolderTick;
        break;
        case Delay:
            return &Effect::delayTick;
        break;
        case LpFilter:
            return &Effect::lowpassTick;
        break;
        case HpFilter:
            return &Effect::highpassTick;
        break;
        case BpFilter:
            return &Effect::bandpassTick;
        break;
        case DLFilter:
            return &Effect::diodeLowpassTick;
        break;
        case VZPFilter:
            return &Effect::VZpeakTick;
        break;
        case VZLFilter:
            return &Effect::VZlowshelfTick;
        break;
        case VZHFilter:
            return &Effect::VZhighshelfTick;
        break;
        case VZBFilter:
            return &Effect::VZbandrejectTick;
        break;
        case LLFilter:
            return &Effect::LadderLowpassTick;
        break;
        case FXTypeNil:
            return &Effect::tick;
        break;
    }
}

//got the idea from https://ccrma.stanford.edu/~jatin/ComplexNonlinearities/Wavefolder.html  -JS
float Effect::wavefolderTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 12.0f);
    float offset = (param2 * 2.0f) - 1.0f;
    sample = (sample * gain) + (offset * gain);
    float curFB = param3;
    float curFF = param4;
    float ff = curFF * tanhf(sample) + ((1.0f - curFF) * sample);
    float fb = curFB * tanhf(wfState[v]);
    wfState[v] = (ff + fb) - param5 * sinf(TWO_PI * sample);
    sample = wfState[v] / (1.0f + curFB);
    sample = tHighpass_tick(&dcBlock1[v], sample);
    //sample *= param5[v];
    return sample;
}

float Effect::delayTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    
    float FBval = param2 * 1.1f;
    FBval = LEAF_clip(0.0f, FBval, 0.9f);
    
    float cutoff1 = LEAF_clip(0.0f, (param3* 127.0f-16.0f) * 35.929824561403509f, 4095.0f); //get in proper range for setFreqFast
    float cutoff2 = LEAF_clip(0.0f, (param4*127.0f-16.0f) * 35.929824561403509f, 4095.0f); //get in proper range for setFreqFast
    tSVF_setFreqFast(&delayLP[v], cutoff1);
    tSVF_setFreqFast(&delayHP[v], cutoff2);
    

    sample = tFeedbackLeveler_tick(&feedbackControl[v], tanhf(sample + (delayFB[v] * FBval)));

    tTapeDelay_setDelay(&tapeDelay[v], param1 * 30000.0f);
    
    if (param5 > 0.5f)
    {
        delayFB[v] = tTapeDelay_tick(&tapeDelay[v], sample);
    }
    
    else
    {
        delayFB[v] = tTapeDelay_tick(&tapeDelay[v], delayFB[v]);
    }
    
    delayFB[v] = tSVF_tick(&delayLP[v], delayFB[v]);
    delayFB[v] = tanhf(tSVF_tick(&delayHP[v], delayFB[v]));
    sample = delayFB[v];
    return sample;
}

float Effect::chorusTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float baseLength = param1 * 5780.0f + 10.0f;
    float modDepth = param2 * 0.1f;
    float modSpeed1 = param3 * 0.4f + 0.01f;
    float modSpeed2 = param4 * 0.4444444f + 0.01f;
    tCycle_setFreq(&mod1[v], modSpeed1);
    tCycle_setFreq(&mod2[v], modSpeed2);
    
    tHermiteDelay_setDelay(&delay1[v], baseLength * .707f * (1.0f + modDepth * tCycle_tick(&mod1[v])));
    tHermiteDelay_setDelay(&delay2[v], baseLength * .5f * (1.0f - modDepth * tCycle_tick(&mod2[v])));
    float temp = tHermiteDelay_tick(&delay1[v], sample) - sample;
    temp += tHermiteDelay_tick(&delay2[v], sample) - sample;
    
    //temp = tHighpass_tick(&dcBlock1[v], temp);
    return temp;
}

float Effect::shaperTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 24.0f);
    float offset = (param2 * 2.0f) - 1.0f;
    sample = sample * gain;
    float temp = LEAF_shaper(sample + (offset * gain),param3);
    temp = tHighpass_tick(&dcBlock1[v], temp);
    return temp;
}

float Effect::tick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    return sample;
}

float Effect::tiltFilterTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    tVZFilter_setGain(&shelf1[v], fastdbtoa(-1.0f * ((param1 * 30.0f) - 15.0f)));
    tVZFilter_setGain(&shelf2[v], fastdbtoa((param1 * 30.0f) - 15.0f));
    tVZFilter_setFrequencyAndBandwidthAndGain(&bell1[v], faster_mtof(param2 * 77.0f + 42.0f), (param3 +1.0f)*6.0f, fastdbtoa((param4* 34.0f) - 17.0f));
    sample = tVZFilter_tickEfficient(&shelf1[v], sample);
    sample = tVZFilter_tickEfficient(&shelf2[v], sample);
    sample = tVZFilter_tickEfficient(&bell1[v], sample);
    return sample;
}

void Effect::oversample_tick(float* samples, int v)
{
    float param1 = quickParams[Param1][v]->read();
    float param2 = quickParams[Param2][v]->read();
    float param3 = quickParams[Param3][v]->read();
    float param4 = quickParams[Param4][v]->read();
    float param5 = quickParams[Param5][v]->read();
    float mix = quickParams[Mix][v]->read();
    float postGain = quickParams[PostGain][v]->read();
    for(int i = 0; i < OVERSAMPLE; i++)
    {
        float output = (this->*_tick)((samples[i]), param1, param2, param3, param4, param5, v);
        samples[i] = ((1.0f - mix) * (samples[i])) + (mix * output);
        samples[i] *= fasterdbtoa((postGain * 24.0f) - 12.0f);
    }
    sampleInBlock++;
}

float Effect::tanhTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 24.0f);
    sample = sample * gain;
    gain = gain * 0.5f;
    float offset = (param2 * 2.0f) - 1.0f;
    //need to do something with shape param
    float temp = tanhf(sample + (offset*gain));
    temp = tHighpass_tick(&dcBlock1[v], temp);
    temp = tanhf(temp);
    //temp = tHighpass_tick(&dcBlock2, temp);
    return temp;
}

float Effect::softClipTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 24.0f);
    sample = sample * gain;
    float offset = (param2 * 2.0f) - 1.0f;
    sample = sample + (offset);
    float shape = (param3 * .99f) + 0.01f;
    if (sample <= -1.0f)
    {
        sample = -1.0f;
    } else if (sample >= 1.0f)
    {
        sample = 1.0f;
    }
    {
        sample = (shape * sample) - ((shape * (sample * sample * sample))* 0.3333333f);
        sample = sample / (shape - ((shape*shape*shape) * 0.3333333f));
    }

    sample = tHighpass_tick(&dcBlock1[v], sample);
    return sample;
}


float Effect::hardClipTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    
    float gain = dbtoa(param1 * 24.0f);
    sample = sample * gain;
    float offset = (param2 * 2.0f) - 1.0f;
    sample = sample + (offset);
    float shape = ((param3 * .99f) + 0.01f)* PI_DIV_2;
    if (sample <= -1.0f)
    {
        sample = -1.0f;
    } else if (sample >= 1.0f)
    {
        sample = 1.0f;
    }
    {
        sample = sinf(  (sinf(sample*shape)/sinf(shape)) * shape   );
        sample = sample / sinf(shape);
    }

    sample = tHighpass_tick(&dcBlock1[v], sample);
    return sample;
}


float Effect::satTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 24.0f);
    sample = sample * gain;
    float offset = (param2 * 2.0f) - 1.0f;
    float temp = (sample + (param2 * gain)) / (1.0f + fabs(sample + offset));
    temp = tHighpass_tick(&dcBlock1[v], temp);
    temp = tHighpass_tick(&dcBlock2[v], temp);
    temp = tanhf(temp);
    return temp;
}



float Effect::bcTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    float gain = dbtoa(param1 * 24.0f);
    sample = sample * gain;
    tCrusher_setQuality (&bc[v],param2);
    tCrusher_setSamplingRatio (&bc[v],((1.0f - param3)* inv_oversample) + 0.01f);
    tCrusher_setRound(&bc[v], param4);
    tCrusher_setOperation(&bc[v], param5);
    float temp = tCrusher_tick(&bc[v], sample);
    return temp;
}


float Effect::compressorTick(float sample, float param1, float param2, float param3, float param4, float param5, int v)
{
    
    tCompressor_setParams(&comp[v], param1*-24.0f, ((param2*10.0f)+1.0f), 3.0f, param3 * 18.0f, (param4 * 1000.0f) +  1.0f, (param5 * 1000.0f) + 1.0f);
    return tCompressor_tick (&comp[v], sample);
}



float Effect::scaleFilterResonance(float input)
{
    //lookup table for filter res
    input = LEAF_clip(0.1f, input, 1.0f);
    //scale to lookup range
    input *= 2047.0f;
    int inputInt = (int)input;
    float inputFloat = (float)inputInt - input;
    int nextPos = LEAF_clip(0, inputInt + 1, 2047);
    return LEAF_clip(0.1f, (resTable[inputInt] * (1.0f - inputFloat)) + (resTable[nextPos] * inputFloat), 10.0f);
    //return
}
//filters
float Effect::lowpassTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    
    tSVF_setFreqAndQ(&lowpass[v], faster_mtof(cutoff * 127.f), scaleFilterResonance(q));
    return tSVF_tick(&lowpass[v], sample);
}

float Effect::highpassTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tSVF_setFreqAndQ(&highpass[v], faster_mtof(cutoff * 127.f), scaleFilterResonance(q));
    return tSVF_tick(&highpass[v], sample);
}

float Effect::bandpassTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tSVF_setFreqAndQ(&bandpass[v], faster_mtof(cutoff * 127.f),  scaleFilterResonance(q));
    return tSVF_tick(&bandpass[v], sample);
}

float Effect::diodeLowpassTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tDiodeFilter_setFreq(&diodeFilters[v], faster_mtof(cutoff * 127.f));
    tDiodeFilter_setQ(&diodeFilters[v], scaleFilterResonance(q));
    return tDiodeFilter_tick(&diodeFilters[v], sample);
}

float Effect::LadderLowpassTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tLadderFilter_setFreq(&Ladderfilter[v], faster_mtof(cutoff * 127.f));
    tLadderFilter_setQ(&Ladderfilter[v],  scaleFilterResonance(q));
    return tLadderFilter_tick(&Ladderfilter[v], sample);
}

float Effect::VZlowshelfTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tVZFilter_setFrequencyAndResonanceAndGain(&VZfilterLS[v], faster_mtof(cutoff * 127.f), scaleFilterResonance(q), fasterdbtoa((gain * 50.f) - 25.0f));
    return tVZFilter_tickEfficient(&VZfilterLS[v], sample);
}

float Effect::VZhighshelfTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tVZFilter_setFrequencyAndResonanceAndGain(&VZfilterHS[v], faster_mtof(cutoff * 127.f), scaleFilterResonance(q), fasterdbtoa((gain * 50.0f) - 25.0f));
    return tVZFilter_tickEfficient(&VZfilterHS[v], sample);
}

float Effect::VZpeakTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tVZFilter_setFrequencyAndBandwidthAndGain(&VZfilterPeak[v], faster_mtof(cutoff * 127.f),  scaleFilterResonance(q), dbtoa((gain * 50.f) - 25.f));
    return tVZFilter_tickEfficient(&VZfilterPeak[v], sample);
}

float Effect::VZbandrejectTick(float sample, float cutoff, float gain, float q, float param4,float param5, int v)
{
    tVZFilter_setFrequencyAndResonanceAndGain(&VZfilterBR[v], faster_mtof(cutoff * 127.f), scaleFilterResonance(q), dbtoa((gain * 50.f) - 25.f));
    return tVZFilter_tickEfficient(&VZfilterBR[v], sample);
}

void Effect::loadAll(int v)
{
    quickParams[Param1][v]->setValueToRaw();
    quickParams[Param2][v]->setValueToRaw();
    quickParams[Param3][v]->setValueToRaw();
    quickParams[Param4][v]->setValueToRaw();
    quickParams[Param5][v]->setValueToRaw();
    quickParams[Mix][v]->setValueToRaw();
    quickParams[PostGain][v]->setValueToRaw();
}
