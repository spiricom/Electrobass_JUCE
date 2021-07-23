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
#include "Output.h"

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
    bool midiChannelIsActive(int channel);
    
    //==============================================================================
    bool getMPEMode();
    void setMPEMode(bool enabled);
    
    //==============================================================================
    void sendCopedentMidiMessage();
    void sendPresetMidiMessage();
    
    //==============================================================================
    void addMappingSource(MappingSourceModel* source);
    void addMappingTarget(MappingTargetModel* source);
    
    MappingSourceModel* getMappingSource(const String& name);
    MappingTargetModel* getMappingTarget(const String& name);
    
    //==============================================================================
    File loadWaveTables(const String& setName, File& file);
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    float editorScale = 1.05f;
    
    MidiKeyboardState keyboardState;
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    Array<File> waveTableFiles;
    HashMap<String, Array<tWaveTableS>> waveTables;

    LEAF leaf;
    float voiceNote[NUM_STRINGS];
    
    OwnedArray<Oscillator> oscs;
    OwnedArray<Filter> filt;
    OwnedArray<Envelope> envs;
    OwnedArray<LowFreqOscillator> lfos;
    std::unique_ptr<Output> output;
    
    OwnedArray<SmoothedParameter> pitchBendParams;
    OwnedArray<SmoothedParameter> ccParams;
    OwnedArray<MappingSourceModel> ccSources;
    std::unique_ptr<SmoothedParameter> seriesParallel;

    OwnedArray<SmoothedParameter> params;
    
    HashMap<String, MappingSourceModel*> sourceMap;
    HashMap<String, MappingTargetModel*> targetMap;
    
    struct Mapping
    {
        String sourceName;
        String targetName;
        float value;
    };
    
    Array<Mapping> initialMappings;
    
    std::atomic<float>* pedalValues[CopedentColumnNil];
    
    Array<Array<float>> copedentArray;
    float copedentFundamental;
    String copedentName = "";
    int copedentNumber = 0;
    
    int channelToString[NUM_CHANNELS+1];
    
    int numVoicesActive = 12;
    
    // Must be at least as large of the number of unique skews
    Array<float> invParameterSkews;
    float quickInvParameterSkews[MAX_NUM_UNIQUE_SKEWS];
    int numInvParameterSkews;
    
private:
    
    StringArray paramIds;
    StringArray sourceIds;
    AudioProcessorValueTreeState vts;
    
    char dummy_memory[1];
    tSimplePoly strings[NUM_STRINGS];
    
    float centsDeviation[12];
    int currentTuning;
    int keyCenter = 0;
    
    bool waitingToSendCopedent = false;
    bool waitingToSendPreset = false;
    
    bool mpeMode = true;
    
    int midiChannelNoteCount[NUM_CHANNELS+1];
    int midiChannelActivity[NUM_CHANNELS+1];
    int midiChannelActivityTimeout;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessor)
};
