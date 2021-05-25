/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Utilities.h"
#include "Oscillators.h"
#include "Filters.h"
#include "Envelopes.h"

class StandalonePluginHolder;

//==============================================================================
class ESAudioProcessor : public AudioProcessor,
                         public MidiKeyboardStateListener
{
public:
    //==============================================================================
    ESAudioProcessor();
    ~ESAudioProcessor() override;
    
    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
 
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
    void pitchBend(int channel, int data);
    void ctrlInput(int channel, int ctrl, int value);
    void sustainOn();
    void sustainOff();
    void toggleBypass();
    void toggleSustain();
    
    //==============================================================================
    void sendCopedentMidiMessage();
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    float editorScale = 1.05f;
    
    MidiKeyboardState keyboardState;
    
    StringArray wavetablePaths;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    LEAF leaf;
    float voiceNote[NUM_VOICES];
    float voiceFreq[NUM_VOICES];
    
    std::unique_ptr<SmoothedParameter> masterVolume;
    
    OwnedArray<SawPulseOscillator> sposcs;
    OwnedArray<LowpassFilter> lps;
    OwnedArray<Envelope> envs;
    OwnedArray<LowFreqOscillator> lfos;
    
    OwnedArray<SmoothedParameter> voiceAmpParams;
    OwnedArray<SmoothedParameter> pitchBendParams;
    OwnedArray<SmoothedParameter> ccParams;
    
    OwnedArray<SmoothedParameter> params;
    
    Array<Array<float>> copedentArray;
    
private:
    
    AudioProcessorValueTreeState vts;
    
    char dummy_memory[1];
    tSimplePoly voice[NUM_VOICES];
    
    float centsDeviation[12];
    int currentTuning;
    int keyCenter;
    
    bool waitingToSendCopedent = false;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessor)
};
