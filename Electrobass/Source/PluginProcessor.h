/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Electro_backend/Utilities.h"
#include "Electro_backend/Oscillators.h"
#include "Electro_backend/Filters.h"
#include "Electro_backend/Envelopes.h"
#include "Electro_backend/Output.h"
#include "Electro_backend/TuningControl.hpp"
#include "Electro_backend/Effect.h"
#include "RingBuffer.h"


class StandalonePluginHolder;

//==============================================================================
class ElectroAudioProcessor : public AudioProcessor,
                         public MidiKeyboardStateListener
{
public:
    //==============================================================================
    ElectroAudioProcessor();
    ~ElectroAudioProcessor() override;
    
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
    void sendTuningMidiMessage(String name, int number
                               );
    //==============================================================================
    void addMappingSource(MappingSourceModel* source);
    void addMappingTarget(MappingTargetModel* source);
    
    void tickKnobsToSmooth();
    
    MappingSourceModel* getMappingSource(const String& name);
    MappingTargetModel* getMappingTarget(const String& name);
    
    //==============================================================================
    File loadWaveTables(const String& setName, File& file);
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void addToKnobsToSmoothArray(SmoothedParameter* param);
    //==============================================================================
    float editorScale = 1.05f;
    String wavTableFolder;
    MidiKeyboardState keyboardState;
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    Array<File> waveTableFiles;
    HashMap<String, Array<tWaveTableS>> waveTables;

    LEAF leaf;
    float voiceNote[MAX_NUM_VOICES];
    float voicePrevBend[MAX_NUM_VOICES];
    int highByteVolume;
    Array<SmoothedParameter*> knobsToSmooth;
    OwnedArray<Oscillator> oscs;
    std::unique_ptr<NoiseGenerator> noise;
    OwnedArray<Filter> filt;
    OwnedArray<Envelope> envs;
    OwnedArray<LowFreqOscillator> lfos;
    std::unique_ptr<Output> output;
    OwnedArray<Effect> fx;
    std::unique_ptr<SmoothedParameter> transposeParam;
    OwnedArray<SmoothedParameter> pitchBendParams;
    std::unique_ptr<SmoothedParameter> _pitchBendRange;
    
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
    
    bool voiceIsSounding[MAX_NUM_VOICES];
    
    int numVoicesActive = 1;
    
    // Must be at least as large of the number of unique skews
    Array<float> invParameterSkews;
    float quickInvParameterSkews[MAX_NUM_UNIQUE_SKEWS];
    int numInvParameterSkews;
    
    HashMap<String, int> sourceMappingCounts;
    
    tSimplePoly strings[MAX_NUM_VOICES];
    
    bool pedalControlsMaster = false;
    
    // +1 because we'll treat pedal as 2 macros for ccs
    int macroCCNumbers[NUM_MACROS+1];
    HashMap<int, int> ccNumberToMacroMap;
    
    // +1 because 0 no string/global pitch bend
    int stringChannels[MAX_NUM_VOICES+1];
    HashMap<int, int> channelToStringMap;
    
    float centsDeviation[NUM_MIDI_NOTES];

    StringArray macroNames;
    TuningControl tuner;
    FileChooser* chooser;
    String wavFolder = "";
    PropertySet settings;

    File getLastFile() const
    {
        File f;
        f = File (settings.getValue ("lastStateFile"));
        
        if (f == File())
            f = File::getSpecialLocation (File::userDocumentsDirectory);
        
        return f;
    }
    
    void setLastFile (const FileChooser& fc)
    {
        settings.setValue ("lastStateFile", fc.getResult().getFullPathName());
    }
    
    void setPeakLevel(int channelIndex, float peakLevel);
    float getPeakLevel(int channelIndex);
    
    std::unique_ptr<NormalisableRange<float>> pitchBendRange;
    float convertFrom0to1Func(float value0To1)
    {
        if (_pitchBendRange)
        {
            
            float range  = (_pitchBendRange->getRawValue() + _pitchBendRange->getRawValue());
            float a = (range)*(value0To1);
            float val = (( a / 1)) - _pitchBendRange->getRawValue();
            return val;
        }
        else return 0;
    }
    float convertTo0To1Func(float worldValue)
    {
        if (_pitchBendRange )
        {
            float a = worldValue + _pitchBendRange->getRawValue();
            float val = a / (_pitchBendRange->getRawValue() + _pitchBendRange->getRawValue());
            return val;
        }
        else return 0;
    }
    AudioBufferQueue<float>& getAudioBufferQueue() noexcept { return audioBufferQueue; }
    
    void setPresetName(String name)
    {
        presetName = name;
    }
    
    void setPresetNumber(int number)
    {
        presetNumber = number;
    }
    
    std::unique_ptr<SmoothedParameter> master;
    float oscAmpMult;
    float oscAmpMultArr[4] = {0,1, 0.5, .3333f};
    void setMute(bool _mute) {mute = _mute;}
    
    inline float getOpenString(int i)
    {
        return openStrings[i];
    }
    inline void setOpenString(float val, int i)
    {
        openStrings[i] = val;
    }
    void sendOpenStringMidiMessage();
private:
    
    float openStrings[4] = {28, 33, 38, 43};
    String tuningName;
    int tuningNumber;
    bool mute = false;
    std::atomic<float>* fxPost;
    std::mutex m;
    MTSClient *client;
    StringArray paramIds;
    StringArray sourceIds;
    AudioProcessorValueTreeState vts;
    
    char dummy_memory[1];
    
    int currentTuning;
    int keyCenter = 0;
    
    bool waitingToSendCopedent = false;
    bool waitingToSendPreset = false;
    bool waitingToSendTuning = false;
    bool waitingToSendOpenString = false;
    bool mpeMode = false;
    
    int stringActivity[MAX_NUM_VOICES+1];
    int stringActivityTimeout;
    
    String presetName;
    int presetNumber;
    tOversampler os[MAX_NUM_VOICES];
    float oversamplerArray[OVERSAMPLE];
    AlertWindow prompt;
    std::array<std::atomic<float>, 128> m_peakLevels;
    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector{ audioBufferQueue };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroAudioProcessor)
};
