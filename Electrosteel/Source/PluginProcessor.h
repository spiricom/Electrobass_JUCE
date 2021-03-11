/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"

class StandalonePluginHolder;

//==============================================================================
/**
*/
class ESAudioProcessor : public AudioProcessor,
                         public MidiKeyboardStateListener
{
public:
    //==============================================================================
    ESAudioProcessor();
    ~ESAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    float processTick();
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    //==============================================================================
    void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    //==============================================================================
    void handleMidiMessage(const MidiMessage& message);
    void noteOn(int channel, int key, float velocity);
    void noteOff(int channel, int key, float velocity);
//    void pitchBend(int data);
    void sustainOn();
    void sustainOff();
    void toggleBypass();
    void toggleSustain();
    void ctrlInput(int ctrl, int value);
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void calcVoiceFreq(int voice);
    
    float editorScale = 1.0f;
    
    MidiKeyboardState keyboardState;
    
    StringArray wavetablePaths;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
private:
    
    AudioProcessorValueTreeState valueTreeState;
    
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    char small_memory[1];
    LEAF leaf;
    
    tSimplePoly voice[NUM_VOICES];
    
    tSawtooth osc[NUM_VOICES * NUM_OSC_PER_VOICE];
    tRosenbergGlottalPulse glottal[NUM_VOICES * NUM_OSC_PER_VOICE];
    
    tEfficientSVF synthLP[NUM_VOICES];
    uint16_t filtFreqs[NUM_VOICES];
    tADSRT polyEnvs[NUM_VOICES];
    tADSRT polyFiltEnvs[NUM_VOICES];
    tCycle pwmLFO1;
    tCycle pwmLFO2;
    
    HashMap<String, std::atomic<float>*> params;
    
    float freq[NUM_VOICES];
    float centsDeviation[12];
    int currentTuning;
    int keyCenter;
    
    Array<std::atomic<float>*> pitchBends;
    
    float synthDetune[NUM_VOICES * NUM_OSC_PER_VOICE];
    
    float expBuffer[EXP_BUFFER_SIZE];
    float expBufferSizeMinusOne;
    
    float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];
    float decayExpBufferSizeMinusOne;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessor)
};
