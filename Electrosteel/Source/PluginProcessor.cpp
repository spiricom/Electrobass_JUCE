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
valueTreeState (*this, nullptr, juce::Identifier ("Parameters"), createParameterLayout())
{
    keyboardState.addListener(this);
    
    LEAF_init(&leaf, 44100.0f, small_memory, 1, []() {return (float)rand() / RAND_MAX; });
    
    leaf.clearOnAllocation = 1;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        tSimplePoly_init(&voice[i], 1, &leaf);
        freq[i] = 220.0f;
    }
    
    //exponential buffer rising from 0 to 1
    LEAF_generate_exp(expBuffer, 1000.0f, -1.0f, 0.0f, -0.0008f, EXP_BUFFER_SIZE);
    
    // exponential decay buffer falling from 1 to
    LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE);
    
    //    displayValues[0] = knobs[0]; //synth volume
    //
    //    displayValues[1] = knobs[1] * 4096.0f; //lowpass cutoff
    //
    //    displayValues[2] = knobs[2]; //keyfollow filter cutoff
    //
    //    displayValues[3] = knobs[3]; //detune
    //
    //    displayValues[4] = (knobs[4] * 2.0f) + 0.4f; //filter Q
    //
    //    displayValues[5] = expBuffer[(int)(knobs[5] * expBufferSizeMinusOne)] * 8192.0f; //att
    //
    //    displayValues[6] = expBuffer[(int)(knobs[6] * expBufferSizeMinusOne)] * 8192.0f; //dec
    //
    //    displayValues[7] = knobs[7]; //sus
    //
    //    displayValues[8] = expBuffer[(int)(knobs[8] * expBufferSizeMinusOne)] * 8192.0f; //rel
    //
    //    displayValues[9] = knobs[9]; //leak
    //
    //    displayValues[10] = expBuffer[(int)(knobs[10] * expBufferSizeMinusOne)] * 8192.0f; //att
    //
    //    displayValues[11] = expBuffer[(int)(knobs[11] * expBufferSizeMinusOne)] * 8192.0f; //dec
    //
    //    displayValues[12] = knobs[12]; //sus
    //
    //    displayValues[13] = expBuffer[(int)(knobs[13] * expBufferSizeMinusOne)] * 8192.0f; //rel
    //
    //    displayValues[14] = knobs[14]; //leak
    //
    //    displayValues[15] = knobs[15] * 4095.0f;  // filter envelope amount
    //
    //    displayValues[16] = knobs[16];  // fade between sawtooth and glottal pulse
    
    expBufferSizeMinusOne = EXP_BUFFER_SIZE - 1;
    decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
        {
            int k = (i * NUM_OSC_PER_VOICE) + j;
            tSawtooth_init(&osc[k], &leaf);
            synthDetune[k] = ((leaf.random() * 0.0264f) - 0.0132f);
            tRosenbergGlottalPulse_init(&glottal[k], &leaf);
            tRosenbergGlottalPulse_setOpenLength(&glottal[k], 0.3f);
            tRosenbergGlottalPulse_setPulseLength(&glottal[k], 0.4f);
        }
        
        tEfficientSVF_init(&synthLP[i], SVFTypeLowpass, 2000, 0.4f, &leaf);
        tADSRT_init(&polyEnvs[i], expBuffer[0] * 8192.0f,
                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
        tADSRT_setLeakFactor(&polyEnvs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
        
        tADSRT_init(&polyFiltEnvs[i], expBuffer[0] * 8192.0f,
                    expBuffer[(int)(0.06f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.9f * expBufferSizeMinusOne)] * 8192.0f,
                    expBuffer[(int)(0.1f * expBufferSizeMinusOne)] * 8192.0f,
                    decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
        tADSRT_setLeakFactor(&polyFiltEnvs[i], ((1.0f - 0.1f) * 0.00005f) + 0.99995f);
    }
    tCycle_init(&pwmLFO1, &leaf);
    tCycle_init(&pwmLFO2, &leaf);
    tCycle_setFreq(&pwmLFO1, 0.63f);
    tCycle_setFreq(&pwmLFO2, 0.7211f);
    leaf.clearOnAllocation = 0;
    
    // Storing pointers to parameter values in structures that are faster to access than the APVTS
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBends.add(valueTreeState.getRawParameterValue("PitchBendCh" + String(i+1)));
    }
    
    for (int i = 0; i < SubtractiveKnobParamNil; ++i)
    {
        String s = cSubtractiveKnobParamNames[i];
        params.set(s, valueTreeState.getRawParameterValue(s));
    }
}

ESAudioProcessor::~ESAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout ESAudioProcessor::createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
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
    LEAF_setSampleRate(&leaf, sampleRate);
//    
//    for (int i = 0; i < NUM_VOICES; i++)
//    {
//        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
//        {
//            int k = (i * NUM_OSC_PER_VOICE) + j;
//            tSawtooth_setSampleRate(&osc[k], sampleRate);
//            tRosenbergGlottalPulse_setSampleRate(&glottal[k], sampleRate);
//        }
//        tADSRT_setSampleRate(&polyEnvs[i], sampleRate);
//        tADSRT_setSampleRate(&polyFiltEnvs[i], sampleRate);
//    }
//    tCycle_setSampleRate(&pwmLFO1, sampleRate);
//    tCycle_setSampleRate(&pwmLFO2, sampleRate);
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

void ESAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    float q = *params[cSubtractiveKnobParamNames[SubtractiveFilterQ]];
    float attack = *params[cSubtractiveKnobParamNames[SubtractiveAttack]];
    float decay = *params[cSubtractiveKnobParamNames[SubtractiveDecay]];
    float sustain = *params[cSubtractiveKnobParamNames[SubtractiveSustain]];
    float release = *params[cSubtractiveKnobParamNames[SubtractiveRelease]];
    float leak = *params[cSubtractiveKnobParamNames[SubtractiveLeak]];
    float filtAttack = *params[cSubtractiveKnobParamNames[SubtractiveFilterAttack]];
    float filtDecay = *params[cSubtractiveKnobParamNames[SubtractiveFilterDecay]];
    float filtSustain = *params[cSubtractiveKnobParamNames[SubtractiveFilterSustain]];
    float filtRelease = *params[cSubtractiveKnobParamNames[SubtractiveFilterRelease]];
    float filtLeak = *params[cSubtractiveKnobParamNames[SubtractiveFilterLeak]];

    for (int i = 0; i < NUM_VOICES; i++)
    {
        tEfficientSVF_setQ(&synthLP[i], (q * 2.0f) + 0.4f);
        tADSRT_setAttack(&polyEnvs[i], expBuffer[(int)(attack * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setDecay(&polyEnvs[i], expBuffer[(int)(decay * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setSustain(&polyEnvs[i], sustain);
        tADSRT_setRelease(&polyEnvs[i], expBuffer[(int)(release * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setLeakFactor(&polyEnvs[i], ((1.0f - leak) * 0.00005f) + 0.99995f);
        
        tADSRT_setAttack(&polyFiltEnvs[i], expBuffer[(int)(filtAttack * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setDecay(&polyFiltEnvs[i], expBuffer[(int)(filtDecay * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setSustain(&polyFiltEnvs[i], filtSustain);
        tADSRT_setRelease(&polyFiltEnvs[i], expBuffer[(int)(filtRelease * expBufferSizeMinusOne)] * 8192.0f);
        tADSRT_setLeakFactor(&polyFiltEnvs[i], ((1.0f - filtLeak) * 0.00005f) + 0.99995f);
    }
    
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float sample = processTick();
        
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, i, sample);
        }
    }
}

float ESAudioProcessor::processTick()
{
    float sample = 0.0f;
    
    float volume = *params[cSubtractiveKnobParamNames[SubtractiveVolume]];
    float detune = *params[cSubtractiveKnobParamNames[SubtractiveDetune]];
    float shape = *params[cSubtractiveKnobParamNames[SubtractiveShape]];
    float keyFollow = *params[cSubtractiveKnobParamNames[SubtractiveFilterKeyFollow]];
    float cutoff = *params[cSubtractiveKnobParamNames[SubtractiveFilterCutoff]];
    float amount = *params[cSubtractiveKnobParamNames[SubtractiveFilterAmount]];
    
    //==============================================================================
    float tempLFO1 = (tCycle_tick(&pwmLFO1) * 0.25f) + 0.5f; // pulse length
    float tempLFO2 = ((tCycle_tick(&pwmLFO2) * 0.25f) + 0.5f) * tempLFO1; // open length
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        float midiNote = tSimplePoly_getPitch(&voice[i], 0);
        
        calcVoiceFreq(i);
        
        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
        {
            int k = (i * NUM_OSC_PER_VOICE) + j;
            float tempFreq = freq[i] * (1.0f + (synthDetune[k] * detune));
            tSawtooth_setFreq(&osc[k], tempFreq);
            tRosenbergGlottalPulse_setFreq(&glottal[k], tempFreq);
            tRosenbergGlottalPulse_setPulseLength(&glottal[k], tempLFO1);
            tRosenbergGlottalPulse_setOpenLength(&glottal[k], tempLFO2);
        }
        
        float keyFollowFilt = midiNote * keyFollow * 64.0f;
        float tempFreq2 = cutoff * 4096.0f + keyFollowFilt;
        tempFreq2 = LEAF_clip(0.0f, tempFreq2, 4095.0f);
        filtFreqs[i] = (uint16_t) tempFreq2;

        float tempSample = 0.0f;
        float env = tADSRT_tick(&polyEnvs[i]);
        
        for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
        {
            int k = (i * NUM_OSC_PER_VOICE) + j;
            tempSample += tSawtooth_tick(&osc[k]) * env * (1.0f - shape);
            tempSample += tRosenbergGlottalPulse_tickHQ(&glottal[k]) * env * shape;
        }
        float f = (amount * 4095.0f * tADSRT_tick(&polyFiltEnvs[i])) + filtFreqs[i];
        tEfficientSVF_setFreq(&synthLP[i], LEAF_clip(0.0f, f, 4095.0f));
        sample += tEfficientSVF_tick(&synthLP[i], tempSample);
    }

    sample *= INV_NUM_OSC_PER_VOICE * volume;
    sample = tanhf(sample);
    return sample;
}

//==============================================================================
bool ESAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ESAudioProcessor::createEditor()
{
    return new ESAudioProcessorEditor (*this, valueTreeState);
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
    if (m.isNoteOnOrOff())
    {
        if (m.getChannel() > 1) keyboardState.processNextMidiEvent(m);
    }
    else if (m.isPitchWheel())
    {
        // Parameters should be set with a 0. to 1. range,
        float bend = m.getPitchWheelValue() / 16383.0f;
        valueTreeState.getParameter("PitchBendCh" + String(m.getChannel()))->setValueNotifyingHost(bend);
    }
    else if (m.isController())
    {
        
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
            tADSRT_on(&polyEnvs[i], velocity);
            tADSRT_on(&polyFiltEnvs[i], velocity);
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
        tADSRT_off(&polyEnvs[i]);
        tADSRT_off(&polyFiltEnvs[i]);
    }
    
    if (tSimplePoly_getNumActiveVoices(&voice[i]) < 1)
    {
        //        setLED_2(vcd, 0);
    }
    
}

//void ESAudioProcessor::pitchBend(int data)
//{
//    pitchBendValue = (data - 8192) * 0.000244140625f;
//}

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

void ESAudioProcessor::ctrlInput(int ctrl, int value)
{
    
}

//==============================================================================
void ESAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
//    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("Electrosteel"));
//
//    xml->setAttribute("editorScale", editorScale);
//
//    auto state = valueTreeState.copyState();
//    xml->addChildElement(state.createXml().get());
//
//    copyXmlToBinary (*xml, destData);
    
    auto state = valueTreeState.copyState();
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
//        forEachXmlChildElementWithTagName(*xmlState, parameters, valueTreeState.state.getType())
//        {
//            valueTreeState.replaceState(ValueTree::fromXml(*parameters));
//        }
//    }
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
        
        editorScale = xmlState->getDoubleAttribute("editorScale", 1.0);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ESAudioProcessor();
}

//==============================================================================
void ESAudioProcessor::calcVoiceFreq(int v)
{
    float pitchBend = *pitchBends[0] + *pitchBends[v+1];
    float tempNote = (float)tSimplePoly_getPitch(&voice[v], 0) + pitchBend;
    float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
    float tunedNote = tempNote + centsDeviation[(int)tempPitchClass];
    freq[v] = LEAF_midiToFrequency(tunedNote);
}



