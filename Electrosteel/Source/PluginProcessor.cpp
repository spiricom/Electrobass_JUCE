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
AudioProcessorValueTreeState::ParameterLayout ESAudioProcessor::createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    //==============================================================================
    // Top level parameters
    
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        layout.add (std::make_unique<AudioParameterFloat> ("M" + String(i+1),
                                                           "M" + String(i+1),
                                                           0., 1., 0.));
    }
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        layout.add (std::make_unique<AudioParameterFloat> ("PitchBend" + String(i),
                                                           "PitchBend" + String(i),
                                                           -24., 24., 0.));
    }
    
    //==============================================================================
    for (int i = 0; i < cOscParams.size(); ++i)
    {
        float min = vOscInit[i][0];
        float max = vOscInit[i][1];
        float def = vOscInit[i][2];
        for (int j = 0; j < NUM_OSCS; ++j)
        {
            String n = "Osc" + String(j+1) + cOscParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        String n = "Osc" + String(i+1);
        layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 1));
        n = "Osc" + String(i+1) + "ShapeSet";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, oscSetNames, 0));
        n = "Osc" + String(i+1) + "FilterSend";
        layout.add (std::make_unique<AudioParameterFloat> (n, n, 0.f, 1.f, 0.5f));
    }
    
    //==============================================================================
    for (int i = 0; i < cFilterParams.size(); ++i)
    {
        float min = vFilterInit[i][0];
        float max = vFilterInit[i][1];
        float def = vFilterInit[i][2];
        for (int j = 0; j < NUM_FILT; ++j)
        {
            String n = "Filter" + String(j+1) + cFilterParams[i];
            layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
        }
    }
    for (int i = 0; i < NUM_FILT; ++i)
    {
        String n = "Filter" + String(i+1);
        layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 1));
        n = "Filter" + String(i+1) + "Type";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, oscSetNames, 0));
    }
    
    layout.add (std::make_unique<AudioParameterFloat> ("Filter Series-Parallel Mix",
                                                       "Filter Series-Parallel Mix",
                                                       0., 1., 0.));
    
    //==============================================================================
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
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        String n = "Envelope" + String(i+1) + "Velocity";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 1));
    }
    
    //==============================================================================
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
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        String n = "LFO" + String(i+1) + "Sync";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 0));
        n = "LFO" + String(i+1) + "ShapeSet";
        layout.add (std::make_unique<AudioParameterChoice> (n, n, oscSetNames, 0));
    }
    
    //==============================================================================
    for (int i = 0; i < cOutputParams.size(); ++i)
    {
        float min = vOutputInit[i][0];
        float max = vOutputInit[i][1];
        float def = vOutputInit[i][2];
        String n = "Output" + cOutputParams[i];
        layout.add (std::make_unique<AudioParameterFloat> (n, n, min, max, def));
    }
    
    layout.add (std::make_unique<AudioParameterFloat> ("Master", "Master",
                                                       0., 2., 1.));
    
    //==============================================================================
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
    
    tSimplePoly_init(&strings[0], 12, &leaf);
    tSimplePoly_setNumVoices(&strings[0], 1);
    for (int i = 1; i < NUM_STRINGS; i++)
    {
        tSimplePoly_init(&strings[i], 1, &leaf);
        voiceFreq[i] = 220.0f;
    }

    leaf.clearOnAllocation = 0;
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscs.add(new Oscillator("Osc" + String(i+1), *this, vts));
    }
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filt.add(new Filter("Filter" + String(i+1), *this, vts));
    }
    
    afpSeriesParallel = vts.getRawParameterValue("Filter Series-Parallel Mix");
    
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        envs.add(new Envelope("Envelope" + String(i+1), *this, vts));
    }
    
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        lfos.add(new LowFreqOscillator("LFO" + String(i+1), *this, vts));
    }
    
    output = std::make_unique<Output>("Output", *this, vts);
    
    // Make a SmoothedParameter for each voice from the single AudioParameter
    // to allow for env mapping
    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        output->getParameter(OutputAmp)[i]->setHook(2, &(envs[NUM_ENVS-1]->getValuePointer()[i]),
                                                    0.0, 1.0, HookAdd);
        filt[0]->getParameter(FilterCutoff)[i]->setHook(2, envs[NUM_ENVS-1]->getValuePointer(),
                                                       0.f, 5000.f, HookAdd);
    }
    
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        ccParams.add(new SmoothedParameter(*this, vts, "M" + String(i+1)));
    }
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBendParams.add(new SmoothedParameter(*this, vts, "PitchBend" + String(i)));
        midiChannelNoteCount[i+1] = 0;
        midiChannelActivity[i+1] = 0;
        channelToString[i+1] = i;
    }
    
    //==============================================================================
    
    for (int i = 0; i < CopedentColumnNil; ++i)
    {
        copedentArray.add(Array<float>());
        for (int v = 0; v < NUM_STRINGS; ++v)
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
void ESAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    midiChannelActivityTimeout = sampleRate/samplesPerBlock/2;
    LEAF_setSampleRate(&leaf, sampleRate);
    for (auto param : params)
    {
        param->setSampleRate(sampleRate);
    }
    for (auto sposc : oscs)
    {
        sposc->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto lp : filt)
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

union flatUnion
{
    float f;
    uint32_t i;
};

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

        
        //RangedAudioParameter* fund = vts.getParameter("Copedent Fundamental");
        //flat.add(fund->convertFrom0to1(fund->getValue()));
        

        Array<uint8_t> flat7bitInt;
        
        union flatUnion fu;
        
        for (int j =0; j < 11; j++)
        {
            flat7bitInt.clear();
            
            flat7bitInt.add(1); // saying it's a copedent
            flat7bitInt.add(1); // saying which copedent number to store (need this to be a user entered value
            flat7bitInt.add(50 + j);
            
            for (int i = 0; i < 12; i++)
            {
                fu.f = flat[i + (j*12)];
                flat7bitInt.add((fu.i >> 28) & 15);
                flat7bitInt.add((fu.i >> 21) & 127);
                flat7bitInt.add((fu.i >> 14) & 127);
                flat7bitInt.add((fu.i >> 7) & 127);
                flat7bitInt.add(fu.i & 127);
            }
            
            
            
            MidiMessage copedentMessage = MidiMessage::createSysExMessage(flat7bitInt.getRawDataPointer(), sizeof(uint8_t) * flat7bitInt.size());
            
            midiMessages.addEvent(copedentMessage, 0);
        }
        waitingToSendCopedent = false;
    }
    
    Array<float> resolvedCopedent;
    for (int r = 0; r < NUM_STRINGS; ++r)
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
    
    for (int i = 0; i < oscs.size(); ++i)
    {
        oscs[i]->frame();
    }
    for (int i = 0; i < filt.size(); ++i)
    {
        filt[i]->frame();
    }
    for (int i = 0; i < envs.size(); ++i)
    {
        envs[i]->frame();
    }
    for (int i = 0; i < lfos.size(); ++i)
    {
        lfos[i]->frame();
    }

    float parallel = *afpSeriesParallel;
    
    int mpe = mpeMode ? 1 : 0;
    int impe = 1-mpe;
    
    float samples[2], filterSamples[2];
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        for (int p = 0; p < NUM_MACROS; ++p)
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
        samples[0] = 0.f;
        samples[1] = 0.f;
        float globalPitchBend = pitchBendParams[0]->tick();
        for (int v = 0; v < NUM_STRINGS; ++v)
        {
            float pitchBend = globalPitchBend + pitchBendParams[v+1]->tick();
            float tempNote = (float)tSimplePoly_getPitch(&strings[v*mpe], v*impe);
            tempNote += resolvedCopedent[v];
            tempNote += pitchBend;
            float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
            float tunedNote = tempNote + centsDeviation[(int)tempPitchClass];
            voiceNote[v] = tunedNote;
            voiceFreq[v] = LEAF_midiToFrequency(tunedNote);
            if (voiceFreq[v] < 10.0f) voiceFreq[v] = 0.0f;
            
            filterSamples[0] = 0.f;
            filterSamples[1] = 0.f;
            for (int i = 0; i < oscs.size(); ++i)
            {
                oscs[i]->tick(v, filterSamples);
            }
            
            float filter1 = filt[0]->tick(v, filterSamples[0]);
            float sample = filt[1]->tick(v, filterSamples[1] + filter1*(1.f-parallel))
            + filter1*parallel;
            output->tick(v, sample, samples, totalNumOutputChannels);
        }
    
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, i, samples[channel]);
        }
    }
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
        if (midiChannelActivity[i] > 0) midiChannelActivity[i]--;
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
    if (mpeMode) midiChannelNoteCount[midiChannel]++;
    noteOn(midiChannel, midiNoteNumber, velocity);
}

void ESAudioProcessor::handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (mpeMode) midiChannelNoteCount[midiChannel]--;
    noteOff(midiChannel, midiNoteNumber, velocity);
}

//==============================================================================
void ESAudioProcessor::handleMidiMessage(const MidiMessage& m)
{
    if (m.isNoteOnOrOff())
    {
        keyboardState.processNextMidiEvent(m);
    }
    else
    {
        int channel = mpeMode ? m.getChannel() : 1;
        if (mpeMode) midiChannelActivity[channel] = midiChannelActivityTimeout;
        if (m.isPitchWheel())
        {
            pitchBend(channel, m.getPitchWheelValue());
        }
        else if (m.isController())
        {
            ctrlInput(channel, m.getControllerNumber(), m.getControllerValue());
        }
    }
}

void ESAudioProcessor::noteOn(int channel, int key, float velocity)
{
    int i = mpeMode ? channelToString[channel]-1 : 0;
    if (i < 0) return;
    if (!velocity) noteOff(channel, key, velocity);
    else
    {
        int v = tSimplePoly_noteOn(&strings[i], key, velocity * 127.f);
        if (!mpeMode) i = v;
        
        if (v >= 0)
        {
            for (auto e : envs) e->noteOn(i, velocity);
            for (auto o : lfos) o->noteOn(i, velocity);
        }
    }
}

void ESAudioProcessor::noteOff(int channel, int key, float velocity)
{
    int i = mpeMode ? channelToString[channel]-1 : 0;
    if (i < 0) return;
    // if we're monophonic, we need to allow fast voice stealing and returning
    // to previous stolen notes without regard for the release envelopes
    int v = tSimplePoly_noteOff(&strings[i], key);
    if (!mpeMode) i = v;
    
    if (v >= 0)
    {
        for (auto e : envs) e->noteOff(i, velocity);
        for (auto o : lfos) o->noteOff(i, velocity);
    }
}

void ESAudioProcessor::pitchBend(int channel, int data)
{
    // Parameters need to be set with a 0. to 1. range, but will use their set range when accessed
    float bend = data / 16383.f;
    if (mpeMode)
    {
        vts.getParameter("PitchBend" + String(channelToString[channel]))->setValueNotifyingHost(bend);
    }
    else
    {
        vts.getParameter("PitchBend0")->setValueNotifyingHost(bend);
    }
}

void ESAudioProcessor::ctrlInput(int channel, int ctrl, int value)
{
    float v = value / 127.f;
    if (channel == 1)
    {
        if (1 <= ctrl && ctrl <= NUM_MACROS)
        {
            vts.getParameter("M" + String(ctrl))->setValueNotifyingHost(v);
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
bool ESAudioProcessor::midiChannelIsActive(int channel)
{
    return midiChannelNoteCount[channel] + midiChannelActivity[channel] > 0;
}

//==============================================================================
bool ESAudioProcessor::getMPEMode()
{
    return mpeMode;
}

void ESAudioProcessor::setMPEMode(bool enabled)
{
    mpeMode = enabled;
    tSimplePoly_setNumVoices(&strings[0], mpeMode ? 1 : 12);
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
    auto state = vts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    
    xml->setAttribute("editorScale", editorScale);
    xml->setAttribute("mpeMode", mpeMode);
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        xml->setAttribute("Ch" + String(i+1) + "String", channelToString[i+1]);
    }
    
    copyXmlToBinary (*xml, destData);
}

void ESAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (vts.state.getType()))
            vts.replaceState (juce::ValueTree::fromXml (*xmlState));
        
        editorScale = xmlState->getDoubleAttribute("editorScale", 1.05);
        mpeMode = xmlState->getBoolAttribute("mpeMode", true);
        
        for (int i = 0; i < NUM_CHANNELS; ++i)
        {
            channelToString[i+1] = xmlState->getIntAttribute("Ch" + String(i+1) + "String", i);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ESAudioProcessor();
}
