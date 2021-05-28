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
vts(*this, nullptr, juce::Identifier ("Parameters"), createParameterLayout())
{
    keyboardState.addListener(this);
    
    LEAF_init(&leaf, 44100.0f, dummy_memory, 1, []() {return (float)rand() / RAND_MAX; });
    
    leaf.clearOnAllocation = 1;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tSimplePoly_init(&voice[i], 1, &leaf);
        voiceFreq[i] = 220.0f;
    }

    leaf.clearOnAllocation = 0;
    
    
    for (int i = 0; i < 2; ++i)
    {
        String s (i+1);
        sposcs.add(new SawPulseOscillator("SawPulse" + s, *this, vts, cSawPulseParams));
        lps.add(new LowpassFilter("Lowpass" + s, *this, vts, cLowpassParams));
    }
    
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        envs.add(new Envelope("Envelope" + String(i+1), *this, vts, cEnvelopeParams));
    }
    
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        lfos.add(new LowFreqOscillator("LFO" + String(i+1), *this, vts, cLowFreqParams));
    }
    
    // Make a SmoothedParameter for each voice from the single AudioParameter
    // to allow for env mapping
    for (int i = 0; i < NUM_VOICES; ++i)
    {
        voiceAmpParams.add(new SmoothedParameter(*this, vts, "Amp"));
        voiceAmpParams[i]->setHook(0, &(envs[3]->getValuePointer()[i]),
                                   0.0, 1.0, HookAdd);
        lps[0]->getParameter(LowpassCutoff)[i]->setHook(0, envs[0]->getValuePointer(),
                                                        0.f, 5000.f, HookAdd);
    }
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
        ccParams.add(new SmoothedParameter(*this, vts, "CC" + String(i+1)));
    }
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBendParams.add(new SmoothedParameter(*this, vts, "PitchBendCh" + String(i+1)));
    }
    
    masterVolume = std::make_unique<SmoothedParameter>(*this, vts, "Master");
    
    //==============================================================================
    
    for (int i = 0; i < CopedentColumnNil; ++i)
    {
        copedentArray.add(Array<float>());
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            copedentArray.getReference(i).add(cCopedentArrayInit[i][v]);
        }
    }
}

ESAudioProcessor::~ESAudioProcessor()
{
    params.clearQuick(false);
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ESAudioProcessor::createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    //==============================================================================
    // Top level parameters
    layout.add (std::make_unique<AudioParameterFloat> ("Master", "Master",
                                                       0., 2., 1.));
    
    layout.add (std::make_unique<AudioParameterFloat> ("Amp", "Amp",
                                                       0., 2., 0.));
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
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
    //==============================================================================
    
    for (int i = 0; i < cSawPulseParams.size(); ++i)
    {
        float min = vSawPulseInit[i][0];
        float max = vSawPulseInit[i][1];
        float def = vSawPulseInit[i][2];
        for (int j = 0; j < 2 /* NUM SPOSC */; ++j)
        {
            String n = "SawPulse" + String(j+1) + cSawPulseParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    
    for (int i = 0; i < cLowpassParams.size(); ++i)
    {
        float min = vLowpassInit[i][0];
        float max = vLowpassInit[i][1];
        float def = vLowpassInit[i][2];
        for (int j = 0; j < 2 /* NUM LP */; ++j)
        {
            String n = "Lowpass" + String(j+1) + cLowpassParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    
    for (int i = 0; i < cEnvelopeParams.size(); ++i)
    {
        float min = vEnvelopeInit[i][0];
        float max = vEnvelopeInit[i][1];
        float def = vEnvelopeInit[i][2];
        for (int j = 0; j < NUM_ENVS; ++j)
        {
            String n = "Envelope" + String(j+1) + cEnvelopeParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    
    for (int i = 0; i < cLowFreqParams.size(); ++i)
    {
        float min = vLowFreqInit[i][0];
        float max = vLowFreqInit[i][1];
        float def = vLowFreqInit[i][2];
        for (int j = 0; j < NUM_LFOS; ++j)
        {
            String n = "LFO" + String(j+1) + cLowFreqParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    for (int j = 0; j < NUM_LFOS; ++j)
    {
        String n = "LFO" + String(j+1) + "Sync";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 0));
    }
    
    for (int i = 1; i < CopedentColumnNil; ++i)
    {
        layout.add (std::make_unique<AudioParameterChoice>(cCopedentColumnNames[i],
                                                           cCopedentColumnNames[i],
                                                           StringArray("Off", "On"), 0));
    }
    
    layout.add (std::make_unique<AudioParameterFloat>("Copedent Fundamental",
                                                      "Copedent Fundamental",
                                                      0.f, 60.f, 21.f));
    
    return layout;
}

//==============================================================================
void ESAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    LEAF_setSampleRate(&leaf, sampleRate);
    for (auto param : params)
    {
        param->setSampleRate(sampleRate);
    }
    for (auto sposc : sposcs)
    {
        sposc->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto lp : lps)
    {
        lp->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto env : envs)
    {
        env->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto lfo : lfos)
    {
        lfo->prepareToPlay(sampleRate, samplesPerBlock);
    }
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
    
    if (waitingToSendCopedent)
    {
        Array<float> flat;
        for (auto column : copedentArray)
        {
            for (auto value : column)
            {
                flat.add(value);
            }
        }
        RangedAudioParameter* fund = vts.getParameter("Copedent Fundamental");
        flat.add(fund->convertFrom0to1(fund->getValue()));
        
        MidiMessage copedentMessage = MidiMessage::createSysExMessage(flat.getRawDataPointer(), sizeof(float) * flat.size());
        midiMessages.addEvent(copedentMessage, 0);
        waitingToSendCopedent = false;
    }
    
    Array<float> resolvedCopedent;
    for (int r = 0; r < NUM_VOICES; ++r)
    {
        float minBelowZero = 0.0f;
        float maxAboveZero = 0.0f;
        for (int c = 1; c < CopedentColumnNil; ++c)
        {
            if (vts.getParameter(cCopedentColumnNames[c])->getValue() > 0)
            {
                float value = copedentArray.getReference(c)[r];
                if (value < minBelowZero) minBelowZero = value;
                else if (value > maxAboveZero) maxAboveZero = value;
            }
        }
        resolvedCopedent.add(minBelowZero + maxAboveZero);
    }
    
    for (int i = 0; i < envs.size(); ++i)
    {
        envs[i]->frame();
    }
//    for (int i = 0; i < lfos.size(); ++i)
//    {
//        lfos[i]->frame();
//    }
    
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        for (int p = 0; p < NUM_GLOBAL_CC; ++p)
        {
            ccParams[p]->tick();
        }
        for (int i = 0; i < envs.size(); ++i)
        {
            envs[i]->tick();
        }
        for (int i = 0; i < lfos.size(); ++i)
        {
            lfos[i]->tick();
        }
        float sample = 0.f;
        float globalPitchBend = pitchBendParams[0]->tick();
        for (int v = 0; v < NUM_VOICES; ++v)
        {
            float pitchBend = globalPitchBend + pitchBendParams[v+1]->tick();
            float tempNote = (float)tSimplePoly_getPitch(&voice[v], 0);
            tempNote += resolvedCopedent[v];
            tempNote += pitchBend;
            float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
            float tunedNote = tempNote + centsDeviation[(int)tempPitchClass];
            voiceNote[v] = tunedNote;
            voiceFreq[v] = LEAF_midiToFrequency(tunedNote);
            if (voiceFreq[v] < 10.0f) voiceFreq[v] = 0.0f;
            
            float voiceSample = 0.f;
            voiceSample = sposcs[0]->tick(v);
            voiceSample = lps[0]->tick(v, voiceSample);
            voiceSample *= voiceAmpParams[v]->tick();
            sample += voiceSample;
        }
        
        sample *= masterVolume->tick();
        
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
        int v = tSimplePoly_noteOn(&voice[i], key, velocity * 127.f);
        
        if (v >= 0)
        {
            for (auto e : envs) e->noteOn(i, velocity);
            for (auto o : lfos) o->noteOn(i, velocity);
        }
    }
    
    if (tSimplePoly_getNumActiveVoices(&voice[i]) >= 1)
    {
        //        setLED_2(vcd, 0);
    }
}

void ESAudioProcessor::noteOff(int channel, int key, float velocity)
{
    int i = channel-2;
    
    // if we're monophonic, we need to allow fast voice stealing and returning
    // to previous stolen notes without regard for the release envelopes
    int v = tSimplePoly_noteOff(&voice[i], key);
    
    if (v >= 0)
    {
        for (auto e : envs) e->noteOff(i, velocity);
        for (auto o : lfos) o->noteOff(i, velocity);
    }
    
    if (tSimplePoly_getNumActiveVoices(&voice[i]) < 1)
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
void ESAudioProcessor::sendCopedentMidiMessage()
{
    waitingToSendCopedent = true;
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
        
//        editorScale = xmlState->getDoubleAttribute("editorScale", 1.05);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ESAudioProcessor();
}
