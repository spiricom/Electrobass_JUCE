/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "ESStandalone.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ESAudioProcessor::ESAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  )
#endif
,
vts (*this, nullptr, juce::Identifier ("Parameters"), createParameterLayout()),
shared(*this, vts),
subSynth(*this, vts, shared),
tableSynth(*this, vts, shared)
{
    keyboardState.addListener(this);
}

ESAudioProcessor::~ESAudioProcessor()
{
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ESAudioProcessor::createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add (std::make_unique<AudioParameterFloat> ("CC1", "CC1 (Volume)", 0., 1., 1.));
    for (int i = 1; i < NUM_GLOBAL_CC; ++i)
    {
        layout.add (std::make_unique<AudioParameterFloat> ("CC" + String(i+1),
                                                           "CC" + String(i+1),
                                                           0., 1., 0.));
    }
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        layout.add (std::make_unique<AudioParameterFloat> ("PitchBendCh" + String(i+1),
                                                           "PitchBendCh" + String(i+1),
                                                           -24., 24., 0.));
    }
    
    for (int i = 0; i < SubtractiveKnobParamNil; ++i)
    {
        float min = cSubtractiveKnobParamInitValues[i][0];
        float max = cSubtractiveKnobParamInitValues[i][1];
        float def = cSubtractiveKnobParamInitValues[i][2];
        layout.add (std::make_unique<AudioParameterFloat> (cSubtractiveKnobParamNames[i],
                                                           cSubtractiveKnobParamNames[i],
                                                           min, max, def));
    }
    
    for (int i = 0; i < WavetableKnobParamNil; ++i)
    {
        float min = cWavetableKnobParamInitValues[i][0];
        float max = cWavetableKnobParamInitValues[i][1];
        float def = cWavetableKnobParamInitValues[i][2];
        layout.add (std::make_unique<AudioParameterFloat> (cWavetableKnobParamNames[i],
                                                           cWavetableKnobParamNames[i],
                                                           min, max, def));
    }
    
    return layout;
}

//==============================================================================
const juce::String ESAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ESAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ESAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ESAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ESAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ESAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int ESAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ESAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ESAudioProcessor::getProgramName (int index)
{
    return {};
}

void ESAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ESAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    shared.prepareToPlay(sampleRate, samplesPerBlock);
    subSynth.prepareToPlay(sampleRate, samplesPerBlock);
}

void ESAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ESAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void ESAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear (i, 0, buffer.getNumSamples());
    
    MidiMessage m;
    for (MidiMessageMetadata metadata : midiMessages) {
        m = metadata.getMessage();
        handleMidiMessage(m);
    }
    
    subSynth.frame();
    
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float sample = subSynth.tick();
        
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, i, sample);
        }
    }
}

//==============================================================================
bool ESAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ESAudioProcessor::createEditor()
{
    return new ESAudioProcessorEditor (*this, vts);
}

//==============================================================================
void ESAudioProcessor::handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (midiChannel > 1) noteOn(midiChannel, midiNoteNumber, velocity);
}

void ESAudioProcessor::handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (midiChannel > 1) noteOff(midiChannel, midiNoteNumber, velocity);
}

//==============================================================================
void ESAudioProcessor::handleMidiMessage(const MidiMessage& m)
{
    int channel = m.getChannel();
    if (m.isNoteOnOrOff())
    {
        if (channel > 1) keyboardState.processNextMidiEvent(m);
    }
    else if (m.isPitchWheel())
    {
        pitchBend(channel, m.getPitchWheelValue());
    }
    else if (m.isController())
    {
        ctrlInput(channel, m.getControllerNumber(), m.getControllerValue());
    }
}

void ESAudioProcessor::noteOn(int channel, int key, float velocity)
{
    int i = channel-2;
    if (!velocity) noteOff(channel, key, velocity);
    else
    {
        int v = tSimplePoly_noteOn(&shared.voice[i], key, velocity * 127.f);
        
        if (v >= 0)
        {
            subSynth.noteOn(i, velocity);
        }
    }
    
    if (tSimplePoly_getNumActiveVoices(&shared.voice[i]) >= 1)
    {
        //        setLED_2(vcd, 0);
    }
}

void ESAudioProcessor::noteOff(int channel, int key, float velocity)
{
    int i = channel-2;
    
    // if we're monophonic, we need to allow fast voice stealing and returning
    // to previous stolen notes without regard for the release envelopes
    int v = tSimplePoly_noteOff(&shared.voice[i], key);
    
    if (v >= 0)
    {
        subSynth.noteOff(i, velocity);
    }
    
    if (tSimplePoly_getNumActiveVoices(&shared.voice[i]) < 1)
    {
        //        setLED_2(vcd, 0);
    }
}

void ESAudioProcessor::pitchBend(int channel, int data)
{
    // Parameters need to be set with a 0. to 1. range, but will use their set range when accessed
    float bend = data / 16383.f;
    vts.getParameter("PitchBendCh" + String(channel))->setValueNotifyingHost(bend);
}

void ESAudioProcessor::ctrlInput(int channel, int ctrl, int value)
{
    float v = value / 127.f;
    if (channel == 1)
    {
        if (1 <= ctrl && ctrl <= 5)
        {
            vts.getParameter("CC" + String(ctrl))->setValueNotifyingHost(v);
        }
    }
}

void ESAudioProcessor::sustainOff()
{
    
}

void ESAudioProcessor::sustainOn()
{
    
}

void ESAudioProcessor::toggleBypass()
{
    
}

void ESAudioProcessor::toggleSustain()
{
    
}

//==============================================================================
void ESAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
//    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("Electrosteel"));
//
//    xml->setAttribute("editorScale", editorScale);
//
//    auto state = vts.copyState();
//    xml->addChildElement(state.createXml().get());
//
//    copyXmlToBinary (*xml, destData);
    
    auto state = vts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    xml->setAttribute("editorScale", editorScale);
    copyXmlToBinary (*xml, destData);
}

void ESAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
//    if (xmlState.get() != nullptr)
//    {
//        editorScale = xmlState->getDoubleAttribute("editorScale", 1.0);
//        forEachXmlChildElementWithTagName(*xmlState, parameters, vts.state.getType())
//        {
//            vts.replaceState(ValueTree::fromXml(*parameters));
//        }
//    }
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (vts.state.getType()))
            vts.replaceState (juce::ValueTree::fromXml (*xmlState));
        
        editorScale = xmlState->getDoubleAttribute("editorScale", 1.0);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ESAudioProcessor();
}
