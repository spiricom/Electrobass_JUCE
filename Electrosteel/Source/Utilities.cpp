/*
  ==============================================================================

    Utilities.cpp
    Created: 17 Mar 2021 4:28:36pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Utilities.h"
#include "PluginProcessor.h"

SmoothedParameter::SmoothedParameter(ESAudioProcessor& processor, AudioProcessorValueTreeState& vts, String paramId, int voice) :
processor(processor),
voice(voice)
{
    raw = vts.getRawParameterValue(paramId);
    parameter = vts.getParameter(paramId);
    for (int i = 0; i < 3; ++i) hooks[i] = ParameterHook("", &value0, false, 1, 0.0f, 0.0f);
    processor.params.add(this);
}

float SmoothedParameter::tick(int i)
{
    float target = raw->load() +
    hooks[0].getValue(i) + hooks[1].getValue(i) + hooks[2].getValue(i);
    smoothed.setTargetValue(target);
    return value = smoothed.getNextValue();
}

float SmoothedParameter::tickNoHooks()
{
    smoothed.setTargetValue(*raw);
    return value = smoothed.getNextValue();
}

float SmoothedParameter::skip(int numSamples)
{
    float target = raw->load() +
    hooks[0].getValue(numSamples) + hooks[1].getValue(numSamples) + hooks[2].getValue(numSamples);
    smoothed.setTargetValue(target);
    return value = smoothed.skip(numSamples);
}

float SmoothedParameter::get()
{
    return value;
}

float** SmoothedParameter::getValuePointerArray()
{
    return &valuePointer;
}

ParameterHook& SmoothedParameter::getHook(int index)
{
    return hooks[index];
}

void SmoothedParameter::setHook(const String& sourceName, int index,
             const float* hook, int size, float min, float max)
{
    hooks[index].sourceName = sourceName;
    hooks[index].hook = (float*)hook;
    hooks[index].buffered = true;
    hooks[index].size = size;
    hooks[index].min = min;
    hooks[index].length = max-min;
}

void SmoothedParameter::resetHook(int index)
{
    hooks[index].sourceName = "";
    hooks[index].hook = &value0;
    hooks[index].buffered = false;
    hooks[index].size = 1;
    hooks[index].min = 0.0f;
    hooks[index].length = 0.0f;
}

void SmoothedParameter::updateHook(int index, const float* hook)
{
    hooks[index].hook = (float*)hook;
}

void SmoothedParameter::setRange(int index, float min, float max)
{
    hooks[index].min = min;
    hooks[index].length = max-min;
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
    for (int i = 0; i < 3; ++i)
    {
        if (hooks[i].buffered)
            hooks[i].size = samplesPerBlock;
    }
}

//==============================================================================
//==============================================================================

MappingSourceModel::MappingSourceModel(ESAudioProcessor& p, const String &name, float** source,
                                       bool perVoice, bool buffered, bool bipolar, Colour colour) :
name(name),
source(source),
numSourcePointers(perVoice ? NUM_STRINGS : 1),
buffered(buffered),
bipolar(bipolar),
colour(colour),
modelProcessor(p)
{
}

MappingSourceModel::~MappingSourceModel()
{
    
}

float** MappingSourceModel::getValuePointerArray()
{
    return source;
}

int MappingSourceModel::getNumSourcePointers()
{
    return numSourcePointers;
}

int MappingSourceModel::getBufferSize()
{
    return buffered ? modelProcessor.getBlockSize() : 1;
}

//==============================================================================
//==============================================================================

MappingTargetModel::MappingTargetModel(ESAudioProcessor& p, const String &name,
                                       OwnedArray<SmoothedParameter>& targetParameters,
                                       int index) :
processor(p),
name(name),
currentSource(nullptr),
targetParameters(targetParameters),
index(index)
{
}

MappingTargetModel::~MappingTargetModel()
{
}

void MappingTargetModel::prepareToPlay()
{
    // Check for any active hooks
    ParameterHook& hook = targetParameters[0]->getHook(index);
    if (hook.sourceName.isNotEmpty())
    {
        // Remake the hook in case the block size has changed
        MappingSourceModel* source = processor.sourceMap[hook.sourceName];
        setMapping(source, hook.min+hook.length, false);
    }
}

void MappingTargetModel::setMapping(MappingSourceModel* source, float end, bool sendChangeEvent)
{
    if (source == nullptr) return;
    
    currentSource = source;
    bipolar = source->isBipolar();
    
    value = end;
    
    int i = 0;
    int n = source->getNumSourcePointers();
    float start = 0.f;
    for (auto param : targetParameters)
    {
        param->setHook(source->name, index, source->getValuePointerArray()[i%n],
                       source->getBufferSize(), start, end);
        i++;
    }
    
    if (onMappingChange != nullptr) onMappingChange(sendChangeEvent);
}

void MappingTargetModel::removeMapping(bool sendChangeEvent)
{
    currentSource = nullptr;
    
    value = 0.f;
    
    for (auto param : targetParameters)
    {
        param->resetHook(index);
    }
    
    if (onMappingChange != nullptr) onMappingChange(sendChangeEvent);
}

void MappingTargetModel::setMappingRange(float end, bool sendChangeEvent)
{
    value = end;
    float start = 0.f;
    for (auto param : targetParameters)
    {
        param->setRange(index, start, end);
    }
    DBG(String(start) + " " + String(end));
    
    if (onMappingChange != nullptr) onMappingChange(sendChangeEvent);
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
        String pn = name + paramNames[i];
        params.add(new OwnedArray<SmoothedParameter>());
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            params[i]->add(new SmoothedParameter(p, vts, pn, v));
            ref[i][v] = params[i]->getUnchecked(v);
            lastValues[i][v] = 0.f;
            values[i][v] = 0.f;
        }
        for (int t = 0; t < 3; ++t)
        {
            String targetName = pn + "T" + String(t+1);
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
