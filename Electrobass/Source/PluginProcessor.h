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
    bool stringIsActive(int string);
    
    //==============================================================================
    bool getMPEMode();
    void setMPEMode(bool enabled);
    
    void setNumVoicesActive(int numVoices);
    
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
    float voicePrevBend[NUM_STRINGS];
    int highByteVolume;
    OwnedArray<Oscillator> oscs;
    std::unique_ptr<NoiseGenerator> noise;
    OwnedArray<Filter> filt;
    OwnedArray<Envelope> envs;
    OwnedArray<LowFreqOscillator> lfos;
    std::unique_ptr<Output> output;
    
    std::unique_ptr<SmoothedParameter> transposeParam;
    OwnedArray<SmoothedParameter> pitchBendParams;
    OwnedArray<SmoothedParameter> ccParams;
    OwnedArray<MappingSourceModel> ccSources;
    std::unique_ptr<SmoothedParameter> seriesParallelParam;
    
    float* midiKeyValues[MAX_NUM_UNIQUE_SKEWS];
    std::unique_ptr<MappingSourceModel> midiKeySource;
    int midiKeyMin = 21; // Default to A0
    int midiKeyMax = 108; // Default to C8

	float* velocityValues[MAX_NUM_UNIQUE_SKEWS];
	float lastVelocityValue = 0.f;
	std::unique_ptr<MappingSourceModel> velocitySource;
    
    float* randomValues[MAX_NUM_UNIQUE_SKEWS];
    float lastRandomValue = 0.f;
    std::unique_ptr<MappingSourceModel> randomSource;

    OwnedArray<SmoothedParameter> params;
    
    HashMap<String, MappingSourceModel*> sourceMap;
    HashMap<String, MappingTargetModel*> targetMap;
    
    struct Mapping
    {
        String sourceName = String();
        String scalarName = String();
        String targetName = String();
        float value = 0.f;
    };
    
    Array<Mapping> initialMappings;
    
    std::atomic<float>* pedalValues[CopedentColumnNil];
    
    Array<Array<float>> copedentArray;
    float copedentFundamental;
    String copedentName = "";
    int copedentNumber = 0;
    
    bool voiceIsSounding[NUM_STRINGS];
    
    int numVoicesActive = NUM_STRINGS;
    
    // Must be at least as large of the number of unique skews
    Array<float> invParameterSkews;
    float quickInvParameterSkews[MAX_NUM_UNIQUE_SKEWS];
    int numInvParameterSkews;
    
    HashMap<String, int> sourceMappingCounts;
    
    tSimplePoly strings[NUM_STRINGS];
    
    bool pedalControlsMaster = true;
    
    // +1 because we'll treat pedal as 2 macros for ccs
    int macroCCNumbers[NUM_MACROS+1];
    HashMap<int, int> ccNumberToMacroMap;
    
    // +1 because 0 no string/global pitch bend
    int stringChannels[NUM_STRINGS+1];
    HashMap<int, int> channelToStringMap;
    
    StringArray macroNames;
    
private:
    
    StringArray paramIds;
    StringArray sourceIds;

    AudioProcessorValueTreeState vts;
    
    char dummy_memory[1];
    
    float centsDeviation[NUM_STRINGS];
    int currentTuning;
    int keyCenter = 0;
    
    bool waitingToSendCopedent = false;
    bool waitingToSendPreset = false;
    
    bool mpeMode = true;
    
    int stringActivity[NUM_STRINGS+1];
    int stringActivityTimeout;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessor)
};
