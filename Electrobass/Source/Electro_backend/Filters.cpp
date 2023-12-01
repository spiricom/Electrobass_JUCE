/*
 ==============================================================================
 
 Filters.cpp
 Created: 17 Mar 2021 3:10:49pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#include "Filters.h"
#include "../PluginProcessor.h"

//==============================================================================
Filter::Filter(const String& n, ElectroAudioProcessor& p,
                             AudioProcessorValueTreeState& vts) :
AudioComponent(n, p, vts, cFilterParams, true)
{
    setParams();
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tSVF_init(&lowpass[i], SVFTypeLowpass, 2000.f, 0.7f, &processor.leaf);
        tSVF_init(&highpass[i], SVFTypeHighpass, 2000.f, 0.7f, &processor.leaf);
        tSVF_init(&bandpass[i], SVFTypeBandpass, 2000.f, 0.7f, &processor.leaf);
        tDiodeFilter_init(&diodeFilters[i], 2000.f, 1.0f, &processor.leaf);
        tVZFilterBell_init(&VZfilterPeak[i], 2000.f, 1.9f, 1.0f, &processor.leaf);
        tVZFilterLS_init(&VZfilterLS[i], 2000.f,  0.6f, 1.0f, &processor.leaf);
        tVZFilterHS_init(&VZfilterHS[i], 2000.f, 0.6f, 1.0f, &processor.leaf);
        tVZFilter_init(&VZfilterBR[i], BandReject, 2000.f, 1.0f, &processor.leaf);
        tLadderFilter_init(&Ladderfilter[i], 2000.f, 1.0f, &processor.leaf);
    }
    
    afpFilterType = vts.getRawParameterValue(n + " Type");
}

Filter::~Filter()
{
    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        tSVF_free(&lowpass[i]);
        tSVF_free(&highpass[i]);
        tSVF_free(&bandpass[i]);
        tDiodeFilter_free(&diodeFilters[i]);
        tVZFilterBell_free(&VZfilterPeak[i]);
        tVZFilterLS_free(&VZfilterLS[i]);
        tVZFilterHS_free(&VZfilterHS[i]);
        tVZFilter_free(&VZfilterBR[i]);
        tLadderFilter_free(&Ladderfilter[i]);
    }
}

void Filter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    AudioComponent::prepareToPlay(sampleRate, samplesPerBlock);
}
//refactor to change function pointer when the type is selected rather than checking at every frame

void Filter::frame()
{
    sampleInBlock = 0;
    enabled = afpEnabled == nullptr || *afpEnabled > 0;
    FilterType type = FilterType(int(*afpFilterType));
    if (type != currentFilterType)
    {
        setAllVariables = true;
    }
    currentFilterType = FilterType(int(*afpFilterType));
    switch (currentFilterType) {
        case LowpassFilter:
            filterTick = &Filter::lowpassTick;
            break;
            
        case HighpassFilter:
            filterTick = &Filter::highpassTick;
            break;
            
        case BandpassFilter:
            filterTick = &Filter::bandpassTick;
            break;
            
        case DiodeLowpassFilter:
            filterTick = &Filter::diodeLowpassTick;
            break;
            
        case VZPeakFilter:
            filterTick = &Filter::VZpeakTick;
            break;
            
        case VZLowshelfFilter:
            filterTick = &Filter::VZlowshelfTick;
            break;
            
        case VZHighshelfFilter:
            filterTick = &Filter::VZhighshelfTick;
            break;
            
        case VZBandrejectFilter:
            filterTick = &Filter::VZbandrejectTick;
            break;
            
        case LadderLowpassFilter:
            filterTick = &Filter::LadderLowpassTick;
            break;
            
        default:
            filterTick = &Filter::lowpassTick;
            break;
    }
}

void Filter::tick(float* samples)
{
    if (!enabled) return;
    
//    float a = sampleInBlock * invBlockSize;
    
    for (int v = 0; v < processor.numVoicesActive; ++v)
    {
        float midiCutoff = quickParams[FilterCutoff][v]->read();
        float keyFollow = quickParams[FilterKeyFollow][v]->read();
        float q = quickParams[FilterResonance][v]->read();
        float gain = quickParams[FilterGain][v]->read();
        //if (processor.knobsToSmooth.contains(quickParams[FilterResonance][v]))
        {
            setQ(q, v);
        }
        //if (processor.knobsToSmooth.contains(quickParams[FilterGain][v]))
        {
            setGain(gain, v);
        }
        //LEAF_clip(0.f, keyFollow, 1.f);
        
        float follow = processor.voiceNote[v];
        if (isnan(follow))
        {
            follow = 0.0f;
        }

        float cutoff = midiCutoff + (follow * keyFollow);
        if (isnan(cutoff))
        {
            cutoff = 60.0f;
        }
        q = q < 0.1f ? 0.1f : q;
        
        (this->*filterTick)(samples[v], v, cutoff, q, keyFollow, gain);
        if (isnan(samples[v]))
        {
            samples[v] = 0.0f;
        }
    }
    
    sampleInBlock++;
}
void Filter::loadAll(int v)
{
    quickParams[FilterGain][v]->setValueToRaw();
    quickParams[FilterCutoff][v]->setValueToRaw();
    quickParams[FilterResonance][v]->setValueToRaw();
    quickParams[FilterGain][v]->setValueToRaw();
    setQ(quickParams[FilterResonance][v]->read(),v);
    setGain(quickParams[FilterGain][v]->read(),v);
}

void Filter::lowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tSVF_setFreqFast(&lowpass[v], cutoff);
    sample = tSVF_tick(&lowpass[v], sample);
    sample *= G;
}

void Filter::highpassTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    //tVZFilter_setMorphOnly(&filters[v], morph);
    //tVZFilter_setFreqAndBandwidth(&filters[v], cutoff + 200.0f, q + 0.01f);
    //sample = tVZFilter_tick(&filters[v], sample);
    tSVF_setFreqFast(&highpass[v], cutoff);
    sample = tSVF_tick(&highpass[v], sample);
    sample *= G;
}

void Filter::bandpassTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    //tVZFilter_setMorphOnly(&filters[v], morph);
    //tVZFilter_setFreqAndBandwidth(&filters[v], cutoff + 200.0f, q + 0.01f);
    //sample = tVZFilter_tick(&filters[v], sample);
    tSVF_setFreqFast(&bandpass[v], cutoff);
    
    sample = tSVF_tick(&bandpass[v], sample);
    sample *= G;
}

void Filter::diodeLowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tDiodeFilter_setFreqFast(&diodeFilters[v], cutoff);
    
    sample = tDiodeFilter_tickEfficient(&diodeFilters[v], sample);
    sample *= G;
}

void Filter::VZpeakTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tVZFilterBell_setFreq(&VZfilterPeak[v], cutoff);
    sample = tVZFilterBell_tick(&VZfilterPeak[v], sample);
    //fasterdbtoa((gain * 50.f) - 25.f)
}

void Filter::VZlowshelfTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tVZFilterLS_setFreqFast(&VZfilterLS[v], cutoff);//, q, fasterdbtoa((gain * 50.f) - 25.f));
    sample = tVZFilterLS_tick(&VZfilterLS[v], sample);
}
void Filter::VZhighshelfTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tVZFilterHS_setFreqFast(&VZfilterHS[v], cutoff); //q, fasterdbtoa((gain * 50.f) - 25.f));
    sample = tVZFilterHS_tick(&VZfilterHS[v], sample);
}
void Filter:: VZbandrejectTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tVZFilter_setFreqFast(&VZfilterBR[v], cutoff);//, q, fasterdbtoa((gain * 50.f) - 25.f));
    sample = tVZFilter_tickEfficient(&VZfilterBR[v], sample);
    sample *= G;
}

void Filter:: LadderLowpassTick(float& sample, int v, float cutoff, float q, float morph, float gain)
{
    tLadderFilter_setFreqFast(&Ladderfilter[v], cutoff);
    
    sample = tLadderFilter_tick(&Ladderfilter[v], sample);
    sample *= G;
}

void Filter::setQ(float q, int v)
{
    q = LEAF_clip(0.01f, q, 10.0f);
    switch (currentFilterType) {
        case LowpassFilter:
            tSVF_setQ(&lowpass[v], q);
            break;
            
        case HighpassFilter:
            tSVF_setQ(&highpass[v], q);
            break;
            
        case BandpassFilter:
            tSVF_setQ(&bandpass[v], q);
            break;
            
        case DiodeLowpassFilter:
            tDiodeFilter_setQ(&diodeFilters[v], q);
            break;
            
        case VZPeakFilter:
            tVZFilterBell_setBandwidth(&VZfilterPeak[v], q);
            break;
            
        case VZLowshelfFilter:
            tVZFilterLS_setResonance(&VZfilterLS[v], q);
            break;
            
        case VZHighshelfFilter:
            tVZFilterHS_setResonance(&VZfilterHS[v], q);
            break;
            
        case VZBandrejectFilter:
            tVZFilter_setResonance(&VZfilterBR[v], q);
            break;
            
        case LadderLowpassFilter:
            tLadderFilter_setQ(&Ladderfilter[v], q);
            break;
            
        default:
            break;
    }
}

void Filter::setGain(float gain, int v)
{
    switch (currentFilterType) {
        case LowpassFilter:
            G = fasterdbtoa((gain * 24.0f) - 12.0f);
            break;
            
        case HighpassFilter:
            G = fasterdbtoa((gain * 24.0f) - 12.0f);
            break;
            
        case BandpassFilter:
            G = fasterdbtoa((gain * 24.0f) - 12.0f);
            break;
            
        case DiodeLowpassFilter:
            G = fasterdbtoa((gain * 24.0f) - 12.0f);
            break;
            
        case VZPeakFilter:
            tVZFilterBell_setGain(&VZfilterPeak[v], fasterdbtoa((gain * 50.f) - 25.f));
            break;
            
        case VZLowshelfFilter:
            tVZFilterLS_setGain(&VZfilterLS[v], fasterdbtoa((gain * 50.f) - 25.f));
            break;
            
        case VZHighshelfFilter:
            tVZFilterHS_setGain(&VZfilterHS[v], fasterdbtoa((gain * 50.f) - 25.f));
            break;
            
        case VZBandrejectFilter:
            //tVZFilter_setGain(&VZfilterBR[v], fasterdbtoa((gain * 50.f) - 25.f));
            G = fasterdbtoa((gain * 24.0f) - 12.0f);
            break;
            
        case LadderLowpassFilter:
            G = fasterdbtoa((gain * 24.f) - 12.f);
            break;
            
        default:
            break;
    }
}


void Filter::setParams()
{
//    "Cutoff",
//    "Gain",
//    "Resonance",
//    "KeyFollow"
    for (int i = 0; i < 2; ++i)
    {
        String pn = AudioComponent::name + " " + paramNames[i];
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < MAX_NUM_VOICES; ++v)
        {
           
            params[i]->add(new SmoothedParameter(processor, vts, pn));
            quickParams[i][v] = params[i]->getUnchecked(v);
        }
        for (int t = 0; t < 3; ++t)
        {
            String targetName = pn + " T" + String(t+1);
            targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
            processor.addMappingTarget(targets.getLast());
        }
    }
    String pn = AudioComponent::name + " " + paramNames[2];
    params.add(new OwnedArray<SmoothedParameter>());
    for (int v = 0; v < MAX_NUM_VOICES; ++v)
    {
       
        params[2]->add(new SkewedParameter(processor, vts, pn, 0.01f, 10.0f, 0.5f));
        quickParams[2][v] = params[2]->getUnchecked(v);
    }
    for (int t = 0; t < 3; ++t)
    {
        String targetName = pn + " T" + String(t+1);
        targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
        processor.addMappingTarget(targets.getLast());
    }
    
    pn = AudioComponent::name + " " + paramNames[3];
    params.add(new OwnedArray<SmoothedParameter>());
    for (int v = 0; v < MAX_NUM_VOICES; ++v)
    {
       
        params[3]->add(new SmoothedParameter(processor, vts, pn));
        quickParams[3][v] = params[3]->getUnchecked(v);
    }
    for (int t = 0; t < 3; ++t)
    {
        String targetName = pn + " T" + String(t+1);
        targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
        processor.addMappingTarget(targets.getLast());
    }
}
