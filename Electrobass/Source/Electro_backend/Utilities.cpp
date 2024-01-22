/*
  ==============================================================================

    Utilities.cpp
    Created: 17 Mar 2021 4:28:36pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "Utilities.h"
#include "../PluginProcessor.h"

SmoothedParameter::SmoothedParameter(ElectroAudioProcessor& processor, AudioProcessorValueTreeState& vts,
                                     String paramId) :
processor(processor),
name(paramId),
removeMe(true),
raw(vts.getRawParameterValue(paramId)),
parameter(vts.getParameter(paramId)),
range(parameter->getNormalisableRange()),
value(raw->load(std::memory_order_relaxed)),
smoothed(value),
isSkewed(false)
{
    //DBG(paramId+" " + String(value));
    for (int i = 0; i < 3; ++i)
    {
        hooks[i] = ParameterHook("", &value0, 0.0f, 0.0f, "", &value1);
        smoothedHooks[i] = 0;
        nonSmoothedHooks[i] = 0;
        
    }
    processor.params.add(this);
    
}

float SmoothedParameter::tick()
{
    // Well defined inter-thread behavior PROBABLY shouldn't be an issue here, so
    // the atomic is just slowing us down. memory_order_relaxed seems fastest, marginally
    removeMe = false;
    float target = raw->load(std::memory_order_seq_cst);
    //bool isSmoothed = false;
    for (int i = 0; i < numSmoothedHooks; ++i)
    {
        target += hooks[smoothedHooks[i]].getValue();
    }
    smoothed.setTargetValue(target);
    value = smoothed.getNextValue();
    value = target;
    for (int i = 0; i < numNonSmoothedHooks; ++i)
    {
        value += hooks[nonSmoothedHooks[i]].getValue();
    }
    if ((numSmoothedHooks == 0) && (numNonSmoothedHooks == 0))
    {
        if ((value >= target - 0.0001f) && (value <= target + 0.0001f)) //changed this because equality check on floats isn't reliable
        {
            removeMe = true;
        }
    }
    return value;
}

float SmoothedParameter::tickNoHooks()
{
    smoothed.setTargetValue(raw->load(std::memory_order_relaxed));
    return value = smoothed.getNextValue();
}

float SmoothedParameter::tickNoHooksNoSmoothing()
{
    return value = raw->load(std::memory_order_relaxed);
}


float SmoothedParameter::get()
{
    return value;
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
    bool isBipolar = false;
    if ((sourceName == "Osc1") || (sourceName == "Osc2") || (sourceName == "Osc3") ||
        (sourceName == "LFO1")  || (sourceName == "LFO2")
        ||(sourceName == "LFO3")
        ||(sourceName == "LFO4") )
    {
        isBipolar = true;
    }
    setHookRange(index, min, max, isBipolar);
    DBG("Sethook " + String(max-min));
    if ((sourceName == "Osc1") || (sourceName == "Osc2") || (sourceName == "Osc3"))
    {
        if (numNonSmoothedHooks < 3) nonSmoothedHooks[numNonSmoothedHooks++] = index;
    }
    else
    {
        if (numSmoothedHooks < 3) smoothedHooks[numSmoothedHooks++] = index;
    }
}

void SmoothedParameter::setHookRange(int index, float min, float max, bool isBipolar)
{
    hooks[index].min = min;
    float difference = max-min;
    hooks[index].length = max-min;
    //float multiplier = 1.0f;
    DBG("Sethook max:start   " + String(max));
    DBG("Sethook min:start   " + String(min));
    /*
    if (difference < 0)
    {
        multiplier = -1.0f;
        difference *= -1.0f;
    }
    if (max < min)
    {
        multiplier = -1.0f;
    }
     */
    //DBG("Difference " + String(difference));

    if(isSkewed)
    {
        float newMin = min;
//        DBG("value max " + String(max));
//        DBG("value min " + String(min));
//        DBG("slider at " + String(parameter->getValue()));
        float val = parameter->getValue();
        if (max < 0.f)
        {
            max = getRange().convertFrom0to1(parameter->getValue()) + max;
        }
        if(min != 0.f)
        {
            newMin = getRange().convertFrom0to1(parameter->getValue()) + min;

            val = 0;
        }
       
        float a = getRange().convertTo0to1(LEAF_clip(getRange().start, max, getRange().end));
        float b = getRange().convertTo0to1(LEAF_clip(getRange().start, newMin, getRange().end));
        hooks[index].length = (a - b - val);
        DBG("b   " + String(b));
        DBG("paramval " + String(parameter->getValue()));
        if (isBipolar)
            b = b -  parameter->getValue();
        hooks[index].min = b;
    }
    DBG("Sethook range:   " + String(hooks[index].length));
    DBG("Sethook min:   " + String(hooks[index].min));
    
}

void SmoothedParameter::setHookScalar(const String& scalarName, int index, float* scalar)
{
    hooks[index].scalarName = scalarName;
    hooks[index].scalar = scalar;
}

void SmoothedParameter::resetHook(int index)
{
    if (hooks[index].hook == &value0) return;
    
    bool isSmoothed = true;
    if ((hooks[index].sourceName == "Osc1") || (hooks[index].sourceName == "Osc2") || (hooks[index].sourceName == "Osc3"))
    {
        isSmoothed = false;
    }

    hooks[index].sourceName = "";
    hooks[index].hook = &value0;
    hooks[index].min = 0.0f;
    hooks[index].length = 0.0f;
    hooks[index].scalarName = "";
    hooks[index].scalar = &value1;
    
    if (isSmoothed)
    {
        numSmoothedHooks--;
        // If this hook was at the start or middle of the active
        // hooks list, make sure to move shift the others forward
        // Could do this with a nested loop but whatever...
        if (smoothedHooks[0] == index)
        {
            smoothedHooks[0] = smoothedHooks[1];
            smoothedHooks[1] = smoothedHooks[2];
        }
        else if (smoothedHooks[1] == index)
        {
            smoothedHooks[1] = smoothedHooks[2];
        }
    }
    else
    {
        numNonSmoothedHooks--;
        // If this hook was at the start or middle of the active
        // hooks list, make sure to move shift the others forward
        // Could do this with a nested loop but whatever...
        if (nonSmoothedHooks[0] == index)
        {
            nonSmoothedHooks[0] = nonSmoothedHooks[1];
            nonSmoothedHooks[1] = nonSmoothedHooks[2];
        }
        else if (nonSmoothedHooks[1] == index)
        {
            nonSmoothedHooks[1] = nonSmoothedHooks[2];
        }
        
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
    smoothed.reset(sampleRate, 0.006);
}

//==============================================================================
//==============================================================================

MappingSourceModel::MappingSourceModel(ElectroAudioProcessor& p, const String &name,
                                       bool perVoice, bool bipolar, Colour colour) :
name(name),
numSourcePointers(perVoice ? MAX_NUM_VOICES : 1),
bipolar(bipolar),
colour(colour),
modelProcessor(p)
{
    source = (float*) leaf_alloc(&p.leaf, sizeof(float) * MAX_NUM_VOICES);
    p.addMappingSource(this);
}

MappingSourceModel::~MappingSourceModel()
{
    if (source != nullptr)
        leaf_free(&modelProcessor.leaf, (char*)source);
    source = nullptr;
}

float* MappingSourceModel::getValuePointerArray()
{
    return source;
}

int MappingSourceModel::getNumSourcePointers()
{
    return numSourcePointers;
}

//==============================================================================
//==============================================================================

MappingTargetModel::MappingTargetModel(ElectroAudioProcessor& p, const String &name,
                                       OwnedArray<SmoothedParameter>& targetParameters,
                                       int index) :
processor(p),
name(name),
targetParameters(targetParameters),
index(index)
{
    //invSkew = targetParameters[0]->getInvSkew();
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
        DBG("Start: " + String(start));
        DBG("End: " + String(end));
    }
    
    float* sourceArray =
    source->getValuePointerArray();
    for (auto param : targetParameters)
    {
        param->setHook(source->name, index, &sourceArray[i%n], start, end);
        i++;
        DBG(source->name);
        processor.addToKnobsToSmoothArray(param);
    }
    if(processor.stream)
        processor.setStreamMappingValuesAdd(this, source);
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
        param->setHookRange(index, start, end, bipolar);
    }
    DBG(String(start) + " " + String(end));
    if(processor.stream)
        processor.setStreamMappingValuesAddRange(this);
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
    
    float* sourceArray = source->getValuePointerArray();
    for (auto param : targetParameters)
    {
        param->setHookScalar(source->name, index, &sourceArray[i%n]);
        i++;
    }
    if(processor.stream)
        processor.setStreamMappingValuesAddScalar(this, source);
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
    if(processor.stream)
        processor.setStreamMappingValuesRemove(this);
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
    if(processor.stream)
        processor.setStreamMappingValuesRemoveScalar(this);
    if (onMappingChange != nullptr) onMappingChange(true, sendChangeEvent);
}

//==============================================================================

AudioComponent::AudioComponent(const String& n, ElectroAudioProcessor& p,
                               AudioProcessorValueTreeState& vts, StringArray s,
                               bool toggleable) :
processor(p),
name(n),
vts(vts),
paramNames(s),
toggleable(toggleable)
{
    
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

void AudioComponent::setParams()
{
    for (int i = 0; i < paramNames.size(); ++i)
    {
        String pn = name + " " + paramNames[i];
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
