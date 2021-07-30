/*
  ==============================================================================

    Utilities.cpp
    Created: 17 Mar 2021 4:28:36pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Utilities.h"
#include "PluginProcessor.h"

SmoothedParameter::SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                                     String paramId) :
processor(processor)
{
    raw = vts.getRawParameterValue(paramId);
    parameter = vts.getParameter(paramId);
    range = parameter->getNormalisableRange();
    for (int i = 0; i < 3; ++i)
    {
        hooks[i] = ParameterHook("", &value0, 0.0f, 0.0f, "", &value1);
        whichHooks[i] = 0;
    }
    processor.params.add(this);
    
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        values[i] = value;
        valuePointers[i] = &values[i];
    }
}

float SmoothedParameter::tick()
{
    // Well defined inter-thread behavior PROBABLY shouldn't be an issue here, so
    // the atomic is just slowing us down. memory_order_relaxed seems fastest, marginally
    float target = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < numActiveHooks; ++i)
    {
        target += hooks[whichHooks[i]].getValue();
    }
    smoothed.setTargetValue(target);
    return value = smoothed.getNextValue();
}

float SmoothedParameter::tickNoHooks()
{
    smoothed.setTargetValue(raw->load(std::memory_order_relaxed));
    return value = smoothed.getNextValue();
}

float SmoothedParameter::tickNoSmoothing()
{
    float target = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < numActiveHooks; ++i)
    {
        target += hooks[whichHooks[i]].getValue();
    }
    return value = target;
}

float SmoothedParameter::tickNoHooksNoSmoothing()
{
    return value = raw->load(std::memory_order_relaxed);
}

void SmoothedParameter::tickSkews()
{
    float target = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < numActiveHooks; ++i)
    {
        target += hooks[whichHooks[i]].getValue();
    }
    smoothed.setTargetValue(target);
    value = smoothed.getNextValue();
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        float invSkew = processor.quickInvParameterSkews[i];
        values[i] = powf(value, invSkew);
    }
}

void SmoothedParameter::tickSkewsNoHooks()
{
    smoothed.setTargetValue(raw->load(std::memory_order_relaxed));
    value = smoothed.getNextValue();
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        float invSkew = processor.quickInvParameterSkews[i];
        values[i] = powf(value, invSkew);
    }
}

void SmoothedParameter::tickSkewsNoSmoothing()
{
    float target = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < numActiveHooks; ++i)
    {
        target += hooks[whichHooks[i]].getValue();
    }
    value = target;
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        float invSkew = processor.quickInvParameterSkews[i];
        values[i] = powf(value, invSkew);
    }
}

void SmoothedParameter::tickSkewsNoHooksNoSmoothing()
{
    value = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < processor.numInvParameterSkews; ++i)
    {
        float invSkew = processor.quickInvParameterSkews[i];
        values[i] = powf(value, invSkew);
    }
}

float SmoothedParameter::skip(int numSamples)
{
    float target = raw->load(std::memory_order_relaxed);
    for (int i = 0; i < numActiveHooks; ++i)
    {
        target += hooks[whichHooks[i]].getValue();
    }
    smoothed.setTargetValue(target);
    return value = smoothed.skip(numSamples);
}

float SmoothedParameter::get()
{
    return value;
}

float SmoothedParameter::get(int i)
{
    return values[i];
}

float** SmoothedParameter::getValuePointerArray()
{
    return &valuePointer;
}

float** SmoothedParameter::getValuePointerArray(int i)
{
    return &valuePointers[i];
}

ParameterHook& SmoothedParameter::getHook(int index)
{
    return hooks[index];
}

void SmoothedParameter::setHook(const String& sourceName, int index,
             const float* hook, float min, float max)
{
    hooks[index].sourceName = sourceName;
    hooks[index].hook = (float*)hook;
    hooks[index].min = min;
    hooks[index].length = max-min;
    
    if (numActiveHooks < 3) whichHooks[numActiveHooks++] = index;
}

void SmoothedParameter::setHookRange(int index, float min, float max)
{
    hooks[index].min = min;
    hooks[index].length = max-min;
}

void SmoothedParameter::setHookScalar(const String& scalarName, int index, float* scalar)
{
    hooks[index].scalarName = scalarName;
    hooks[index].scalar = scalar;
}

void SmoothedParameter::resetHook(int index)
{
    if (hooks[index].hook == &value0) return;
    
    hooks[index].sourceName = "";
    hooks[index].hook = &value0;
    hooks[index].min = 0.0f;
    hooks[index].length = 0.0f;
    hooks[index].scalarName = "";
    hooks[index].scalar = &value1;
    
    numActiveHooks--;
    // If this hook was at the start or middle of the active
    // hooks list, make sure to move shift the others forward
    // Could do this with a nested loop but whatever...
    if (whichHooks[0] == index)
    {
        whichHooks[0] = whichHooks[1];
        whichHooks[1] = whichHooks[2];
    }
    else if (whichHooks[1] == index)
    {
        whichHooks[1] = whichHooks[2];
    }
}

void SmoothedParameter::resetHookScalar(int index)
{
    hooks[index].scalarName = "";
    hooks[index].scalar = &value1;
}

float SmoothedParameter::getStart()
{
    return parameter->getNormalisableRange().start;
}

float SmoothedParameter::getEnd()
{
    return parameter->getNormalisableRange().end;
}

void SmoothedParameter::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    smoothed.reset(sampleRate, 0.010);
}

//==============================================================================
//==============================================================================

MappingSourceModel::MappingSourceModel(ESAudioProcessor& p, const String &name,
                                       bool perVoice, bool bipolar, Colour colour) :
name(name),
numSourcePointers(perVoice ? NUM_STRINGS : 1),
bipolar(bipolar),
colour(colour),
modelProcessor(p)
{
    p.addMappingSource(this);
}

MappingSourceModel::~MappingSourceModel()
{
    
}

float** MappingSourceModel::getValuePointerArray(int i)
{
    return sources[i];
}

int MappingSourceModel::getNumSourcePointers()
{
    return numSourcePointers;
}

//==============================================================================
//==============================================================================

MappingTargetModel::MappingTargetModel(ESAudioProcessor& p, const String &name,
                                       OwnedArray<SmoothedParameter>& targetParameters,
                                       int index) :
processor(p),
name(name),
targetParameters(targetParameters),
index(index)
{
    invSkew = targetParameters[0]->getInvSkew();
}

MappingTargetModel::~MappingTargetModel()
{
}

void MappingTargetModel::prepareToPlay()
{
   
}

void MappingTargetModel::setMapping(MappingSourceModel* source, float e, bool sendChangeEvent)
{
    if (source == nullptr) return;
    
    if (currentSource != nullptr) processor.sourceMappingCounts.getReference(currentSource->name)--;
    processor.sourceMappingCounts.getReference(source->name)++;
    
    currentSource = source;
    bipolar = source->isBipolar();
    
    int i = 0;
    int n = source->getNumSourcePointers();
    
    start = 0.f;
    end = e;
    if (bipolar)
    {
        NormalisableRange<float>& range = targetParameters[0]->getRange();
        float center = targetParameters[0]->getRawValue();
        float pCenter = range.convertTo0to1(center);
        float pOffset = range.convertTo0to1(range.getRange().clipValue(center+end)) - pCenter;
        start = range.convertFrom0to1(jlimit(0.f, 1.f, pCenter-pOffset)) - center;
    }
    
    float* sourceArray =
    *source->getValuePointerArray(processor.invParameterSkews.indexOf(invSkew));
    for (auto param : targetParameters)
    {
        param->setHook(source->name, index, &sourceArray[i%n], start, end);
        i++;
    }
    
    if (onMappingChange != nullptr) onMappingChange(true, sendChangeEvent);
}

void MappingTargetModel::setMappingRange(float e, bool sendChangeEvent,
                                         bool directChange, bool sendListenerNotif)
{
    if (currentSource == nullptr) return;
    
    start = 0.f;
    end = e;
    if (bipolar)
    {
        NormalisableRange<float>& range = targetParameters[0]->getRange();
        float center = targetParameters[0]->getRawValue();
        float pCenter = range.convertTo0to1(center);
        float pOffset = range.convertTo0to1(range.getRange().clipValue(center+end)) - pCenter;
        start = range.convertFrom0to1(jlimit(0.f, 1.f, pCenter-pOffset)) - center;
    }
    
    for (auto param : targetParameters)
    {
        param->setHookRange(index, start, end);
    }
    DBG(String(start) + " " + String(end));
    
    if (onMappingChange != nullptr && sendChangeEvent) onMappingChange(directChange, sendListenerNotif);
}

void MappingTargetModel::setMappingScalar(MappingSourceModel* source, bool sendChangeEvent)
{
    if (source == nullptr) return;
    
    if (currentScalarSource != nullptr)
    {
        processor.sourceMappingCounts.getReference(currentScalarSource->name)--;
    }
    processor.sourceMappingCounts.getReference(source->name)++;
    
    currentScalarSource = source;
    
    int i = 0;
    int n = source->getNumSourcePointers();
    
    float* sourceArray = *source->getValuePointerArray(0);
    for (auto param : targetParameters)
    {
        param->setHookScalar(source->name, index, &sourceArray[i%n]);
        i++;
    }
    
    if (onMappingChange != nullptr) onMappingChange(true, sendChangeEvent);
}

void MappingTargetModel::removeMapping(bool sendChangeEvent)
{
    processor.sourceMappingCounts.getReference(currentSource->name)--;
    
    currentSource = nullptr;
    currentScalarSource = nullptr;
    
    start = end = 0.f;
    
    for (auto param : targetParameters)
    {
        param->resetHook(index);
    }
    
    if (onMappingChange != nullptr) onMappingChange(true, sendChangeEvent);
}

void MappingTargetModel::removeScalar(bool sendChangeEvent)
{
    processor.sourceMappingCounts.getReference(currentScalarSource->name)--;
    if (currentScalarSource != nullptr)
    {
        processor.sourceMappingCounts.getReference(currentScalarSource->name)--;
    }
    
    currentScalarSource = nullptr;
    
    for (auto param : targetParameters)
    {
        param->resetHookScalar(index);
    }
    
    if (onMappingChange != nullptr) onMappingChange(true, sendChangeEvent);
}

//==============================================================================

AudioComponent::AudioComponent(const String& n, ESAudioProcessor& p,
                               AudioProcessorValueTreeState& vts, StringArray s,
                               bool toggleable) :
name(n),
processor(p),
vts(vts),
paramNames(s),
toggleable(toggleable)
{
    for (int i = 0; i < paramNames.size(); ++i)
    {
        String pn = name + " " + paramNames[i];
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            params[i]->add(new SmoothedParameter(p, vts, pn));
            quickParams[i][v] = params[i]->getUnchecked(v);
        }
        for (int t = 0; t < 3; ++t)
        {
            String targetName = pn + " T" + String(t+1);
            targets.add(new MappingTargetModel(processor, targetName, *params.getLast(), t));
            processor.addMappingTarget(targets.getLast());
        }
    }
    
    if (toggleable)
    {
        afpEnabled = vts.getRawParameterValue(n);
    }
    else
    {
        afpEnabled = nullptr;
    }
}

AudioComponent::~AudioComponent()
{
}

void AudioComponent::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    invBlockSize = 1.f/currentBlockSize;
    
    for (auto target : targets)
    {
        target->prepareToPlay();
    }
}

OwnedArray<SmoothedParameter>& AudioComponent::getParameterArray(int p)
{
    return *params[p];
}

MappingTargetModel* AudioComponent::getTarget(int paramId, int index)
{
    return targets[paramId*3+index];
}
