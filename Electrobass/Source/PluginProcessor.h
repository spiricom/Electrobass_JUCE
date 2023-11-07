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

union uintfUnion
{
    float f;
    uint32_t i;
};

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
    void sendTuningMidiMessage(String name, int number);
    //==============================================================================
    void addMappingSource(MappingSourceModel* source);
    void addMappingTarget(MappingTargetModel* source);
    
    void tickKnobsToSmooth();
    void removeKnobsToSmooth();
    MappingSourceModel* getMappingSource(const String& name);
    MappingTargetModel* getMappingTarget(const String& name);
    
    //==============================================================================
    File loadWaveTables(const String& setName, File& file);
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void setStateEBP(const void* data, int sizeInBytes, int presetNumber);
    
    void addToKnobsToSmoothArray(SmoothedParameter* param);
    
    void toggleStream()
    {
        stream = !stream;
    }
    bool midiBufferChanged;
    MidiBuffer tempMidiBuffer[2];
    bool whichMidiBuffer;
    void addToMidiBuffer(int streamID, float streamValue)
    {
        union uintfUnion fu;
        Array<uint8_t> data7bitInt;
        data7bitInt.add(3);
        data7bitInt.add(0);
        fu.f = (float)streamID;
        data7bitInt.add((fu.i >> 28) & 15);
        data7bitInt.add((fu.i >> 21) & 127);
        data7bitInt.add((fu.i >> 14) & 127);
        data7bitInt.add((fu.i >> 7) & 127);
        data7bitInt.add(fu.i & 127);
        
        fu.f = LEAF_clip(0.0f, streamValue, 1.0f);
        data7bitInt.add((fu.i >> 28) & 15);
        data7bitInt.add((fu.i >> 21) & 127);
        data7bitInt.add((fu.i >> 14) & 127);
        data7bitInt.add((fu.i >> 7) & 127);
        data7bitInt.add(fu.i & 127);
        
        if (midiBufferChanged)
        {
            tempMidiBuffer[whichMidiBuffer].clear();
            midiBufferChanged = false;
        }
        tempMidiBuffer[whichMidiBuffer].addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
        data7bitInt.clear();
        
        data7bitInt.add(126);
        tempMidiBuffer[whichMidiBuffer].addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
    }
    
    void addToMidiBuffer(int _streamMappingTargetId , uint8_t _streamMappingTargetSlot, uint8_t _streamMappingIdentifier,float _streamMappingValue)
    {
        union uintfUnion fu;
        Array<uint8_t> data7bitInt;
        data7bitInt.add(4);
        data7bitInt.add(0);
        
        fu.f = (float)_streamMappingTargetId;
        data7bitInt.add((fu.i >> 28) & 15);
        data7bitInt.add((fu.i >> 21) & 127);
        data7bitInt.add((fu.i >> 14) & 127);
        data7bitInt.add((fu.i >> 7) & 127);
        data7bitInt.add(fu.i & 127);
        
        data7bitInt.add(_streamMappingTargetSlot);
        data7bitInt.add(_streamMappingIdentifier);
        
        fu.f = (float)_streamMappingValue;
        data7bitInt.add((fu.i >> 28) & 15);
        data7bitInt.add((fu.i >> 21) & 127);
        data7bitInt.add((fu.i >> 14) & 127);
        data7bitInt.add((fu.i >> 7) & 127);
        data7bitInt.add(fu.i & 127);
        if (midiBufferChanged)
        {
            tempMidiBuffer[!whichMidiBuffer].clear();
            midiBufferChanged = false;
        }
        tempMidiBuffer[whichMidiBuffer].addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
        data7bitInt.clear();
        data7bitInt.add(126);
        tempMidiBuffer[whichMidiBuffer].addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
    }
    
    
    bool stream = false;
    float streamValue1 = 0.0f;
    int streamID1 = 0;
    float streamValue2 = 0.0f;
    int streamID2 = 0;
    bool streamSend = false;
    bool streamMapping = false;
    float streamMappingValue = 0.0f;
    float streamMappingTargetId = 0.f;
    uint8_t streamMappingIdentifier = 0;
    uint8_t streamMappingTargetSlot = 0;
    int idFromName(String name)
    {
        auto it = find(paramDestOrder.begin(), paramDestOrder.end(), name);
        int index = 0;
          // If element was found
          if (it != paramDestOrder.end())
          {
              
              // calculating the index
              // of K
            index = it - paramDestOrder.begin();
          }
        int tempId = index + 2;
        return tempId;
    }
    
    void setStreamMappingValuesAdd(MappingTargetModel *target, MappingSourceModel* source)
    {
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        DBG("Stream source " + source->name);
        streamMappingTargetId = idFromName(target->name.substring(0, target->name.length() - 3));
        streamMappingValue = sourceIds.indexOf(source->name);
        streamMappingTargetSlot = target->name.getTrailingIntValue()-1;
        streamMappingIdentifier = 0;
        
        addToMidiBuffer(streamMappingTargetId ,
                        streamMappingTargetSlot, streamMappingIdentifier,
                        streamMappingValue);
        streamMapping = true;
    }
    
    void setStreamMappingValuesAddRange(MappingTargetModel *target)
    {
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        streamMappingTargetId = idFromName(target->name.substring(0, target->name.length() - 3));
     
        streamMappingTargetSlot = target->name.getTrailingIntValue()-1;
        //determine range
        float multiplier = 1.0f;
        const NormalisableRange<float>& range = vts.getParameter(target->name.substring(0, target->name.length() - 3))->getNormalisableRange();
        float tempRange = target->end;
        if (tempRange < 0.0f)
        {
            multiplier = -1.0f;
            tempRange = fabsf(target->end);
        }
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        tempRange = ((tempRange) / (range.end - range.start));
        float finalRange = tempRange * multiplier;
        DBG("Final Range" + String(finalRange));
        streamMappingValue = finalRange;
        streamMappingIdentifier = 1;
        addToMidiBuffer(streamMappingTargetId ,
                        streamMappingTargetSlot, streamMappingIdentifier,
                        streamMappingValue);
        streamMapping = true;
        
    }
    
    void setStreamMappingValuesRemove(MappingTargetModel *target)
    {
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        streamMappingTargetId = idFromName(target->name.substring(0, target->name.length() - 3));
        streamMappingValue = 255;
        streamMappingTargetSlot = target->name.getTrailingIntValue()-1;
        streamMappingIdentifier = 0;
        addToMidiBuffer(streamMappingTargetId ,
                        streamMappingTargetSlot, streamMappingIdentifier,
                        streamMappingValue);
        streamMapping = true;
    }
    
    void setStreamMappingValuesAddScalar(MappingTargetModel* target, MappingSourceModel* source)
    {
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        streamMappingTargetId = idFromName(target->name.substring(0, target->name.length() - 3));
        streamMappingValue = sourceIds.indexOf(source->name);
        streamMappingTargetSlot = target->name.getTrailingIntValue()-1;
        streamMappingIdentifier = 2;
        addToMidiBuffer(streamMappingTargetId ,
                        streamMappingTargetSlot, streamMappingIdentifier,
                        streamMappingValue);
        streamMapping = true;
    }
    
    void setStreamMappingValuesRemoveScalar(MappingTargetModel* target)
    {
        DBG("Stream target " + target->name.substring(0, target->name.length() - 3));
        streamMappingTargetId = idFromName(target->name.substring(0, target->name.length() - 3));
        streamMappingValue = 255;
        streamMappingTargetSlot = target->name.getTrailingIntValue()-1;
        streamMappingIdentifier = 2;
        addToMidiBuffer(streamMappingTargetId ,
                        streamMappingTargetSlot, streamMappingIdentifier,
                        streamMappingValue);
        streamMapping = true;
    }

    //==============================================================================
    float editorScale = 1.05f;
    String wavTableFolder;
    MidiKeyboardState keyboardState;
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    Array<File> waveTableFiles;
    HashMap<String, Array<tWaveTableS>> waveTables;
    float mtofTable[32768];
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
    
    float* midiKeyValues;
    std::unique_ptr<MappingSourceModel> midiKeySource;
    int midiKeyMin = 21; // Default to A0
    int midiKeyMax = 108; // Default to C8

	float* velocityValues;
	float lastVelocityValue = 0.f;
	std::unique_ptr<MappingSourceModel> velocitySource;
    
    float* randomValues;
    float lastRandomValue = 0.f;
    std::unique_ptr<MappingSourceModel> randomSource;

    OwnedArray<SmoothedParameter> params;
    
    HashMap<String, MappingSourceModel*> sourceMap;
    
    
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
    
    String getPresetName()
    {
        return presetName;
    }

    
    void setPresetNumber(int number)
    {
        presetNumber = number;
    }
    
    double getPresetNumber()
    {
        return presetNumber;
    }

    
    std::unique_ptr<SmoothedParameter> master;
    float oscAmpMult;
    float oscAmpMultArr[4] = {0,1, 0.5, .3333f};
    std::atomic<bool> isProcessing;
    void setProcessing(bool processing) {isProcessing = processing;}
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
    std::atomic<float>* fxPost;
private:
    
    void processMIDIDataIfNeeded (MidiBuffer& midiMessages);
    
    
    HashMap<String, MappingTargetModel*> targetMap;
    float openStrings[4] = {28, 33, 38, 43};
    String tuningName;
    int tuningNumber;
    bool mute = false;
    
    std::mutex m;
    MTSClient *client;
    StringArray paramIds;
    StringArray sourceIds;
    AudioProcessorValueTreeState vts;
    tHighpass dcBlockMaster;
    float denormalMult = 1.0f;
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
    int presetNumber = 0;
    tOversampler os[MAX_NUM_VOICES];
    tHighpass dcBlockPreFilt1[MAX_NUM_VOICES];
    tHighpass dcBlockPreFilt2[MAX_NUM_VOICES];
    float oversamplerArray[OVERSAMPLE];
    AlertWindow prompt;
    std::array<std::atomic<float>, 128> m_peakLevels;
    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector{ audioBufferQueue };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroAudioProcessor)
};
