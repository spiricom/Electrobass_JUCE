/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "Electro_backend/ElectroStandalone.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RingBuffer.h"
#include <regex>
//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ElectroAudioProcessor::createParameterLayout()
{
    ///THIS IS THE DEFAULT PRESET
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    String n;
    //==============================================================================
    // Top level parameters
    
    n = "Master";
    auto normRange = NormalisableRange<float>(0., 1.);
    normRange.setSkewForCentre(0.5);
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID { n,  1 }, n, normRange, 1.));
    paramIds.add(n);
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        n = i < NUM_GENERIC_MACROS ? "M" + String(i + 1) : cUniqueMacroNames[i - NUM_GENERIC_MACROS];
        normRange = NormalisableRange<float>(0., 1.);
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange,
                                                           i == NUM_MACROS-1 ? 1.f : 0.f));
        paramIds.add(n);
    }
    
	auto string2FromValueFunction = [](float v, int length)
	{
		String asText(v, 2);
        asText = (v >= 0 ? "+" : "") + asText;
		return length > 0 ? asText.substring(0, length) : asText;
	};

	auto string3FromValueFunction = [](float v, int length)
	{
		String asText(v, 2);
        asText = (v >= 0 ? "+" : "") + asText;
		return length > 0 ? asText.substring(0, length) : asText;
	};

    n = "Transpose";
    normRange = NormalisableRange<float>(-48., 48.);
	normRange.setSkewForCentre(.0);
    layout.add(std::make_unique<AudioParameterFloat>
               (ParameterID { n,  1 }, n, normRange, 0., String(), AudioProcessorParameter::genericParameter,
            string2FromValueFunction));
	paramIds.add(n);
    pitchBendRange = std::make_unique<NormalisableRange<float>>(-48.f,48.f);
    //pitchBendRange->setSkewForCentre(.0);

    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        n = "PitchBend" + String(i);
        NormalisableRange<float> myParamRange = NormalisableRange<float>
    (
        -48.f, 48.f, // ignored in functions below
        [this]( float start, float end, float value0To1 ) // convertFrom0To1Func
        {
            return pitchBendRange->convertFrom0to1(value0To1); //this->convertFrom0to1Func(value0To1);
        },
        [this]( float start, float end, float worldValue ) // convertTo0To1Func
        {
            return pitchBendRange->convertTo0to1(worldValue);
        },
        [this]( float start, float end, float valueToSnap ) // snapToLegalValueFunc
         {
             return pitchBendRange->snapToLegalValue(valueToSnap);
         }

    );
        myParamRange.setSkewForCentre(0.0);
        layout.add (std::make_unique<AudioParameterFloat>
                    (ParameterID { n,  1 }, n, myParamRange, 0., String(), AudioProcessorParameter::genericParameter,
                     string3FromValueFunction));
        paramIds.add(n);
    }
    n = "PitchBendRange";
    normRange = NormalisableRange<float>(1., 48., 1.);
    layout.add (std::make_unique<AudioParameterFloat>
                (ParameterID { n,  1 }, n, normRange, 48., String(), AudioProcessorParameter::genericParameter));
    paramIds.add(n);
 
    //==============================================================================
    n = "Noise";
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 1));
    paramIds.add(n);
    
    for (int j = 0; j < cNoiseParams.size(); ++j)
    {
        float min = vNoiseInit[j][0];
        float max = vNoiseInit[j][1];
        float def = vNoiseInit[j][2];
        float center = vNoiseInit[j][3];
        
        n = "Noise " + cNoiseParams[j];
        
        normRange = NormalisableRange<float>(min, max);
        normRange.setSkewForCentre(center);
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
        paramIds.add(n);
    }
    
    n = "Noise FilterSend";
    normRange = NormalisableRange<float>(0., 1.);
    normRange.setSkewForCentre(.5);
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, 1.f));
    paramIds.add(n);
    
    //==============================================================================
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        
        n = "Osc" + String(i+1);
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), i == 0 ? 1 : 0));
        paramIds.add(n);
        
        for (int j = 0; j < cOscParams.size(); ++j)
        {
            float min = vOscInit[j][0];
            float max = vOscInit[j][1];
            float def = vOscInit[j][2];
            float center = vOscInit[j][3];
            
            n = "Osc" + String(i+1) + " " + cOscParams[j];
            
            normRange = NormalisableRange<float>(min, max);
            normRange.setSkewForCentre(center);
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
            paramIds.add(n);
        }
       
        n = "Osc" + String(i+1) + " isHarmonic";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n,  StringArray("Off", "On"), 1));
        paramIds.add(n);
        
        n = "Osc" + String(i+1) + " isStepped";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n,  StringArray("Off", "On"), 1));
        paramIds.add(n);
        
        n = "Osc" + String(i+1) + " isSync";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n,  StringArray("Off", "On"), 0));
        paramIds.add(n);
        
        n = "Osc" + String(i+1) + " syncType";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n,  StringArray("Off", "On"), 0));
        paramIds.add(n);
        
        n = "Osc" + String(i+1) + " ShapeSet";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, oscShapeSetNames, 0));
        paramIds.add(n);
        
        n = "Osc" + String(i+1) + " FilterSend";
        normRange = NormalisableRange<float>(0., 1.);
        normRange.setSkewForCentre(.5);
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, 1.f));
        paramIds.add(n);
    }
    
    //==============================================================================
    for (int i = 0; i < NUM_FX; ++i)
    {
        //n = "FX" + String(i+1);
        //layout.add (std::make_unique<AudioParameterChoice> (n, n, StringArray("Off", "On"), 1));
        //paramIds.add(n);
        
        n = "Effect" + String(i+1) + " FXType";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, FXTypeNames, 0));
        paramIds.add(n);
        
        for (int j = 0; j < cFXParams.size(); ++j)
        {
            float min = vFXInit[j][0];
            float max = vFXInit[j][1];
            float def = vFXInit[j][2];
            float center = vFXInit[j][3];
            
            n = "Effect" + String(i+1) + " " + cFXParams[j];
            
            normRange = NormalisableRange<float>(min, max);
            normRange.setSkewForCentre(center);
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
            paramIds.add(n);
        }
    }
    //==============================================================================
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        n = "Filter" + String(i+1);
        if (i == 0)
            layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 1));
        else
            layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 0));

        paramIds.add(n);
        
        n = "Filter" + String(i+1) + " Type";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, filterTypeNames, 0));
        paramIds.add(n);
        
        for (int j = 0; j < cFilterParams.size(); ++j)
        {
            float min = vFilterInit[j][0];
            float max = vFilterInit[j][1];
            float def = vFilterInit[j][2];
            float center = vFilterInit[j][3];
            
            n = "Filter" + String(i+1) + " " + cFilterParams[j];
            normRange = NormalisableRange<float>(min, max);
            normRange.setSkewForCentre(center);
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
            paramIds.add(n);
            DBG(n+String(normRange.convertFrom0to1(1.0f)));
            DBG(n+String(normRange.skew));
        }
    }
    
    n = "Filter Series-Parallel Mix";
    normRange = NormalisableRange<float>(0., 1.);
    normRange.setSkewForCentre(.5);
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, 0.));
    paramIds.add(n);
    
    //=============================================================================
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        for (int j = 0; j < cEnvelopeParams.size(); ++j)
        {
            float min = vEnvelopeInit[j][0];
            float max = vEnvelopeInit[j][1];
            float def = vEnvelopeInit[j][2];
            float center = vEnvelopeInit[j][3];
            
            n = "Envelope" + String(i+1) + " " + cEnvelopeParams[j];
            normRange = NormalisableRange<float>(min, max);
            normRange.setSkewForCentre(center);
            //DBG("envelope skew: " + String(normRange.skew + " " + String(i));
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
            paramIds.add(n);
        }
        
        n = "Envelope" + String(i+1) + " Velocity";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 1));
        paramIds.add(n);
    }
    
    //==============================================================================
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        for (int j = 0; j < cLowFreqParams.size(); ++j)
        {
            float min = vLowFreqInit[j][0];
            float max = vLowFreqInit[j][1];
            float def = vLowFreqInit[j][2];
            float center = vLowFreqInit[j][3];
            
            n = "LFO" + String(i+1) + " " + cLowFreqParams[j];
            normRange = NormalisableRange<float>(min, max);
            normRange.setSkewForCentre(center);
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
            paramIds.add(n);
        }
        
        n = "LFO" + String(i+1) + " ShapeSet";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, lfoShapeSetNames,
                                                            SineTriLFOShapeSet));
        paramIds.add(n);
        
        n = "LFO" + String(i+1) + " Sync";
        layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 0));
        paramIds.add(n);
    }
    
    //==============================================================================
    for (int i = 0; i < cOutputParams.size(); ++i)
    {
        float min = vOutputInit[i][0];
        float max = vOutputInit[i][1];
        float def = vOutputInit[i][2];
        float center = vOutputInit[i][3];
        
        n = "Output " + cOutputParams[i];
        normRange = NormalisableRange<float>(min, max);
        normRange.setSkewForCentre(center);
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { n,  1 }, n, normRange, def));
        paramIds.add(n);
    }
    n = "FX Order";
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID { n,  1 }, n, StringArray("Off", "On"), 0));
    paramIds.add(n);
    
    //==============================================================================
    for (int i = 1; i < CopedentColumnNil; ++i)
    {
        n = cCopedentColumnNames[i];
        layout.add (std::make_unique<AudioParameterChoice>(ParameterID { n,  1 }, n, StringArray("Off", "On"), 0));
    }

    
    DBG("PARAMS//");
    for (int i = 0; i < paramIds.size(); ++i)
    {
        DBG("\"" +paramIds[i]+ "\","); //+ ": " + String(i));
    }
    
    
    return layout;
}


//==============================================================================
//==============================================================================
ElectroAudioProcessor::ElectroAudioProcessor()
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
chooser(nullptr),
vts(*this, nullptr, juce::Identifier ("Parameters"), createParameterLayout()),
prompt("","",AlertWindow::AlertIconType::NoIcon)

{
    fxPost = vts.getRawParameterValue("FX Order");
    formatManager.registerBasicFormats();   
    keyboardState.addListener(this);
    
    LEAF_init(&leaf, 44100.0f, dummy_memory, 1, []() {return (float)rand() / RAND_MAX; });
    
    leaf.clearOnAllocation = 1;
    
    tSimplePoly_init(&strings[0], MAX_NUM_VOICES, &leaf);
    tSimplePoly_setNumVoices(&strings[0], numVoicesActive);
    
    voiceNote[0] = 0;
    for (int i = 1; i < MAX_NUM_VOICES; ++i)
    {
        tSimplePoly_init(&strings[i], 1, &leaf);
        voiceNote[i] = 0;
        voiceIsSounding[i] = false;
    }
    
    for (int i = 0; i < 128; ++i)
    {
        centsDeviation[i] = i;
    }

    leaf.clearOnAllocation = 0;
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        String n = "Osc" + String(i+1);
        oscs.add(new Oscillator(n, *this, vts));
        sourceIds.add(n);
    }
    
    oscs[0]->setSyncSource(oscs[2]);
    oscs[1]->setSyncSource(oscs[0]);
    oscs[2]->setSyncSource(oscs[1]);
    noise = std::make_unique<NoiseGenerator>("Noise", *this, vts);
    sourceIds.add("Noise");
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filt.add(new Filter("Filter" + String(i+1), *this, vts));
    }
    
    for (int i = 0; i < MAX_NUM_VOICES; ++i)
    {
        tOversampler_init(&os[i], OVERSAMPLE, 1, &leaf);
    }
    for (int i = 0; i < NUM_FX; i++)
    {
        fx.add(new Effect("Effect" + String(i+1), *this, vts));
    }
    seriesParallelParam = std::make_unique<SmoothedParameter>(*this, vts, "Filter Series-Parallel Mix");
    
    macroCCNumbers[PEDAL_MACRO_ID+1] = NUM_MACROS+1;
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        macroCCNumbers[i] = i+1;
        
        String n;
        Colour c;
        if (i < NUM_GENERIC_MACROS)
        {
            n = "M" + String(i+1);
            macroNames.add(n);
            c = Colours::red.withSaturation(0.9f);
        }
        else
        {
            n = cUniqueMacroNames[i-NUM_GENERIC_MACROS];
            c = Colours::red.withSaturation(0.5f);
        }
        
        ccParams.add(new SmoothedParameter(*this, vts, n));
        ccSources.add(new MappingSourceModel(*this, n, false, false, c));
        if (i < NUM_MACROS-10)
        {
            sourceIds.add(n);
        }
    }
    for (int i = 1; i <= 127; ++i) ccNumberToMacroMap.set(i, -1);
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        ccNumberToMacroMap.set(macroCCNumbers[i], i);
    }
    
    midiKeySource = std::make_unique<MappingSourceModel>(*this, "MIDI Key In",
                                                         true, false, Colours::white);
    sourceIds.add("MIDI Key In");
	velocitySource = std::make_unique<MappingSourceModel>(*this, "Velocity In",
														  true, false, Colours::white);
    sourceIds.add("Velocity In");
	randomSource = std::make_unique<MappingSourceModel>(*this, "Random on Attack",
                                                        true, false, Colours::white);
    sourceIds.add("Random on Attack");
    //midiKeyValues = (float*)leaf_alloc(&leaf, sizeof(float) * MAX_NUM_VOICES);
    midiKeyValues = midiKeySource->getValuePointerArray();

    //velocityValues = (float*)leaf_alloc(&leaf, sizeof(float) * MAX_NUM_VOICES);
    velocityValues = velocitySource->getValuePointerArray();

    //randomValues //= (float*)leaf_alloc(&leaf, sizeof(float) * MAX_NUM_VOICES);
    randomValues = randomSource->getValuePointerArray();
    //randomSource->source = randomValues;
    
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        String n = "Envelope" + String(i+1);
        envs.add(new Envelope(n, *this, vts));
        sourceIds.add(n);
    }
    
    envs[0]->setIsAmpEnv(true);
    
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        String n = "LFO" + String(i+1);
        lfos.add(new LowFreqOscillator(n, *this, vts));
        sourceIds.add(n);
    }
    //add pedals and knee levers at the end of the source array to avoid breaking existing presets
        for (int i = 0; i < NUM_MACROS - NUM_GENERIC_MACROS - 5; ++i)
        {
            String n;
            Colour c;

            n = cUniqueMacroNames[i+5];
            
            sourceIds.add(n);
        }
    output = std::make_unique<Output>("Output", *this, vts);
    
    master = std::make_unique<SmoothedParameter>(*this, vts, "Master");
    transposeParam = std::make_unique<SmoothedParameter>(*this, vts, "Transpose");
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBendParams.add(new SmoothedParameter(*this, vts, "PitchBend" + String(i)));
    }
    
    _pitchBendRange = std::make_unique<SmoothedParameter>(*this, vts, "PitchBendRange");
    //pitchBendRangeDown = std::make_unique<SmoothedParameter>(*this, vts, "PitchBendRangeDown");

    
    for (int i = 1; i <= 16; ++i) channelToStringMap.set(i, -1);
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
    {
        stringChannels[i] = i+1;
        channelToStringMap.set(stringChannels[i], i);
        stringActivity[i] = 0;
    }
    
    //==============================================================================
    
    for (int i = 0; i < CopedentColumnNil; ++i)
    {
        copedentArray.add(Array<float>());
        for (int v = 0; v < MAX_NUM_VOICES; ++v)
        {
            copedentArray.getReference(i).add(cCopedentArrayInit[i][v]);
        }
    }
    copedentFundamental = 21.f;
    
    for (int i = 1; i < CopedentColumnNil; ++i)
    {
        pedalValues[i] = vts.getRawParameterValue(cCopedentColumnNames[i]);
    }

	// A couple of default mappings that will be used if nothing has been saved
	Mapping defaultFilter1Cutoff;
	defaultFilter1Cutoff.sourceName = "Envelope2";
	defaultFilter1Cutoff.targetName = "Filter1 Cutoff T3";
	defaultFilter1Cutoff.value = 24.f;

	Mapping defaultOutputAmp;
	defaultOutputAmp.sourceName = "Envelope1";
	defaultOutputAmp.targetName = "Output Amp T3";
	defaultOutputAmp.value = 1.f;

	initialMappings.add(defaultFilter1Cutoff);
	initialMappings.add(defaultOutputAmp);
    
    DBG("SOURCE//");
    for (int i = 0; i < sourceIds.size(); ++i)
    {
        DBG(sourceIds[i] + ": " + String(i));
        sourceMappingCounts.set(sourceIds[i], 0);
    }
    
    DBG("Post init: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
}


ElectroAudioProcessor::~ElectroAudioProcessor()
{
    DBG("Pre exit: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
    
    for (auto waveTableSet : waveTables)
    {
        for (auto waveTable : waveTableSet)
        {
            tWaveTableS_free(&waveTable);
        }
    }
    
    for (int i = 0; i < MAX_NUM_VOICES; ++i)
    {
        tSimplePoly_free(&strings[i]);
    }
    

//    leaf_free(&leaf, (char*)midiKeyValues);
//    leaf_free(&leaf, (char*)velocityValues);
//    leaf_free(&leaf, (char*)randomValues);

    DBG("Pre clear: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
//    for (auto p : params)
//    {
//        DBG(p->getName());
//    }
    params.clearQuick(false);
    DBG("Pre delete: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
    if (chooser != nullptr)
        delete chooser;
}


//==============================================================================
void ElectroAudioProcessor::addToKnobsToSmoothArray(SmoothedParameter* param)
{
    bool found = false;
    if(!knobsToSmooth.contains(param))
    {
        param->setRemoveMe(false);
        knobsToSmooth.add(param);
        
    }
        
//    for (auto target: targetMap)
//    {
//        for(auto _param : target->targetParameters)
//        {
//            if(_param == param)
//            {
//                found = true;
//
//            }
//
//        }
//    }
//    if(found)
//    {
//        ;
//    }
//    for (auto knob : knobsToSmooth)
//    {
//        DBG("knob being msooth " + String(knob->getName()));
//    }
}
//==============================================================================
void ElectroAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    stringActivityTimeout = sampleRate/samplesPerBlock/2;
    LEAF_setSampleRate(&leaf, sampleRate);
    
    DBG("Pre prepare: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
    

    
    
    for (auto waveTableSet : waveTables)
    {
        for (auto waveTable : waveTableSet)
        {
            tWaveTableS_setSampleRate(&waveTable, sampleRate);
        }
    }
    
    for (auto env : envs)
    {
        env->prepareToPlay(sampleRate, samplesPerBlock);
    }
    envs[0]->setIsAmpEnv(true);
    for (auto lfo : lfos)
    {
        lfo->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto osc : oscs)
    {
        osc->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto f : filt)
    {
        f->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto effect : fx)
    {
        effect->prepareToPlay(sampleRate, samplesPerBlock);
    }
    output->prepareToPlay(sampleRate, samplesPerBlock);
    
    for (auto param : params)
    {
        param->prepareToPlay(sampleRate, samplesPerBlock);
    }
    
    if (!initialMappings.isEmpty()) // First prepareToPlay
    {
        for (Mapping m : initialMappings)
        {
            targetMap[m.targetName]->setMapping(sourceMap[m.sourceName], m.value, false);
            targetMap[m.targetName]->setMappingScalar(sourceMap[m.scalarName], false);
        }
        initialMappings.clear();
    }
    
    DBG("Post prepare: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
}

void ElectroAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ElectroAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

union uintfUnion
{
    float f;
    uint32_t i;
};

void ElectroAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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
    midiMessages.clear();
    if (waitingToSendPreset)
    {
        Array<float> data;
        
        // Parameter values
        // Order is determined in createParameterLayout
        int count = 0;
       // int myCount = 0;
        //first send a count of the number of parameters that will be sent
        data.add(paramDestOrder.size() + 2); //plus midi key values
        data.add(midiKeyMax/127.0f);
        DBG(String(count++)+ ": Midi Key Max: "+ String(midiKeyMax/127.0f));
        data.add(midiKeyMin/127.0f);
        DBG(String(count++)+ ": Midi Key Min: "+ String(midiKeyMin/127.0f));
        for (auto id : paramDestOrder)
        {
            //data.add((float)myCount++);
            //const NormalisableRange<float>& range = vts.getParameter(id)->getNormalisableRange();
            std::regex e("^PitchBend[0-9]+$|^F[0-9]+$|^K[0-9]+$");
            if (!std::regex_match(id.toStdString(), e))
            {
                data.add(vts.getParameter(id)->getValue());
                DBG(String(count++)+ ": " + id + ": "+ String(vts.getParameter(id)->getValue()));
            }
            else
            {
                DBG("skipped");
            }
         
                
                
        }
        
        //mark end of parameter values
        data.add(-2.0f);
        
        //now prepare the mapping data, need to loop through the array to count how many before adding to the final array
        Array<float> tempData;
        int mapCount = 0;
        // Mappings
        DBG("Mappings");
        DBG("Name: sourceparamid, targetparamaid, scalarsource, range ");
        for (auto id : paramDestOrder)
        {
            for (int t = 0; t < 3; ++t)
            {
                String tn = id + " T" + String(t+1);
                if (targetMap.contains(tn))
                {
                    MappingTargetModel* target = targetMap[tn];
                    if (MappingSourceModel* source = target->currentSource)
                    {
                        tempData.add(sourceIds.indexOf(source->name));//SourceID
                        auto it = find(v.begin(), v.end(), K);
                        int index = 0;
                          // If element was found
                          if (it != v.end())
                          {
                              
                              // calculating the index
                              // of K
                            index = it - v.begin();
                          }
                       
                        tempData.add(index);//TargetID
                        //scalarSource -- negative 1 if no source
                        float scalarsource = -1.0f;
                        if (target->currentScalarSource != nullptr)
                        {
                            scalarsource = sourceIds.indexOf(target->currentScalarSource->name);
                        }
                            
                        tempData.add(scalarsource);
                        float multiplier = 1.0f;
                        const NormalisableRange<float>& range = vts.getParameter(id)->getNormalisableRange();
                        float tempRange = target->end;
                        if (tempRange < 0.0f)
                        {
                            multiplier = -1.0f;
                            tempRange = fabsf(target->end);
                        }
                        tempRange = ((tempRange) / (range.end - range.start));
                        tempData.add(tempRange * multiplier);//Mapping range length
                        DBG(tn +": " + String(sourceIds.indexOf(source->name))+ ", " + String(tempId)+", " + String(scalarsource)+ ", " +String(tempRange * multiplier));
                        mapCount++;
                    }
                }
            }
        }
        
        //now send how many mappings will follow
        data.add(mapCount);
        for (int i = 0; i < tempData.size(); i++)
        {
            data.add(tempData[i]);
        }
        
        //mark end of mapping values
        data.add(-3.0f);

        //next things that would be useful to send:
        // string assignment midi notes
        // tunings
        // how many extra wavetables are used and which oscillators are they assigned to
        // 
//        for (int i = 0 ; i < sourceIds.size(); i++)
//        {
//            DBG(String(i) +  ": " + sourceIds[i]);
//        }
        Array<uint8_t> data7bitInt;
        union uintfUnion fu;
        
        uint16_t sizeOfSysexChunk = (64 / 5) - 3;
        int dataToSend = data.size();
        uint16_t currentChunk = 0;
        uint16_t currentDataPointer = 0;
        data7bitInt.add(0); // saying it's a preset
        data7bitInt.add(presetNumber); // which preset are we saving
        
        for (int i = 0; i < presetName.length(); i++)
        {
            data7bitInt.add((presetName.toUTF8()[i] & 127)); //printable characters are in the 0-127 range
            
        }
        uint16 remainingBlanks = 14 - presetName.length();
        for (uint16 i = 0; i < remainingBlanks; i++)
        {
            data7bitInt.add(32);
        }
        //MidiMessage presetMessage = ;
    
        midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);

        currentChunk++;
        
        //now send the macro names (14 characters each)
        for (int i = 0; i < NUM_GENERIC_MACROS; i++)
        {
            data7bitInt.clear();
            data7bitInt.add(0); // saying it's a preset
            data7bitInt.add(presetNumber); // which preset are we saving
        
            //clip macro names to 14 letters if they are longer
            int myLength = 14;
            if (macroNames[i].length() < 14)
            {
                myLength = macroNames[i].length();
            }
            for (int j = 0; j < myLength; j++)
            {
                data7bitInt.add((macroNames[i].toUTF8()[j] & 127)); //printable characters are in the 0-127 range
            }
            remainingBlanks = 14 - myLength;
            for (int  j = 0; j < remainingBlanks; j++)
            {
                data7bitInt.add(32);
            }
            //MidiMessage presetMessage = ;
        
            midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
            currentChunk++;
        }
        while(currentDataPointer < dataToSend)
        {
            data7bitInt.clear();

            data7bitInt.add(0); // saying it's a preset
            data7bitInt.add(presetNumber); // which preset are we saving
            
            //data7bitInt.add(currentChunk); // whichChhunk
            uint16_t toSendInThisChunk;
            uint16_t dataRemaining = dataToSend - currentDataPointer;
            if (dataRemaining < sizeOfSysexChunk)
            {
                toSendInThisChunk = dataRemaining;
            }
            else
            {
                toSendInThisChunk = sizeOfSysexChunk;
            }

            for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
            {
                fu.f = data[i];
                data7bitInt.add((fu.i >> 28) & 15);
                data7bitInt.add((fu.i >> 21) & 127);
                data7bitInt.add((fu.i >> 14) & 127);
                data7bitInt.add((fu.i >> 7) & 127);
                data7bitInt.add(fu.i & 127);

            }
            currentDataPointer = currentDataPointer + toSendInThisChunk;
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        
            midiMessages.addEvent(presetMessage, 0);

            currentChunk++;
        }
        data7bitInt.clear();
        data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
        data7bitInt.add(1); // which preset did we just finish
        MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        midiMessages.addEvent(presetMessage, 0);
        
        /*
        data7bitInt.clear();
        data7bitInt.add(0); // saying it's a preset
        data7bitInt.add(1); // which preset are we saving
        data7bitInt.add(0); // whichChhunk
        for (int i = 0; i < data.size(); i++)
        {
            fu.f = data[i];
            data7bitInt.add((fu.i >> 28) & 15);
            data7bitInt.add((fu.i >> 21) & 127);
            data7bitInt.add((fu.i >> 14) & 127);
            data7bitInt.add((fu.i >> 7) & 127);
            data7bitInt.add(fu.i & 127);
        }
        MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
    
        midiMessages.addEvent(presetMessage, 0);
        
        */
        // Wavetable data
        /*
        // Determine which tables are in use and therefore should be sent
        Array<String> tableSetsToSend;
        for (auto osc : oscs)
        {
            if (osc->getCurrentShapeSet() == UserOscShapeSet)
            {
                tableSetsToSend.addIfNotAlreadyThere(osc->getWaveTableFile().getFullPathName());
            }
        }
        
        // Send out each set of tables
        for (auto setName : tableSetsToSend)
        {
            data.clear();
            data7bitInt.clear();
            
            for (auto table : waveTables.getReference(setName))
            {
                for (int i = 0; i < table->numTables; ++i)
                {
                    data.add(table->sizes[i]);
                    for (int j = 0; j < table->sizes[i]; ++j)
                    {
                        data.add(table->tables[i][j]);
                    }
                }
            }
            
            for (int i = 0; i < data.size(); i++)
            {
                data7bitInt.add(2); // saying it's wavetable data
                
                fu.f = data[i];
                data7bitInt.add((fu.i >> 28) & 15);
                data7bitInt.add((fu.i >> 21) & 127);
                data7bitInt.add((fu.i >> 14) & 127);
                data7bitInt.add((fu.i >> 7) & 127);
                data7bitInt.add(fu.i & 127);
                

            }
            MidiMessage wtMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
            
            midiMessages.addEvent(wtMessage, 0);
        }
    */
        waitingToSendPreset = false;
    }
    
    
    if (waitingToSendTuning)
    {
        uint16_t currentChunk = 0;
        Array<uint8_t> data7bitInt;
        union uintfUnion fu;
        
        data7bitInt.add(1); // saying it's a preset
        data7bitInt.add(tuningNumber); // which preset are we saving
        
        for (int i = 0; i < presetName.length(); i++)
        {
            data7bitInt.add((presetName.toUTF8()[i] & 127)); //printable characters are in the 0-127 range
            
        }
        uint16 remainingBlanks = 14 - presetName.length();
        for (uint16 i = 0; i < remainingBlanks; i++)
        {
            data7bitInt.add(32);
        }
        //MidiMessage presetMessage = ;
    
        midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);

        currentChunk++;
        Array<float> data;
        for (int i = 0; i < 128; i++)
        {
            data.add(centsDeviation[i]);
        }
        
       
        
        uint16_t currentDataPointer = 0;
        uint16_t sizeOfSysexChunk = (64 / 5) - 3;
        int dataToSend = data.size();
        while(currentDataPointer < dataToSend)
        {
            data7bitInt.clear();

            data7bitInt.add(1); // saying it's a tuning
            data7bitInt.add(presetNumber); // which tuning are we saving
            
            //data7bitInt.add(currentChunk); // whichChhunk
            uint16_t toSendInThisChunk;
            uint16_t dataRemaining = dataToSend - currentDataPointer;
            if (dataRemaining < sizeOfSysexChunk)
            {
                toSendInThisChunk = dataRemaining;
            }
            else
            {
                toSendInThisChunk = sizeOfSysexChunk;
            }

            for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
            {
                fu.f = data[i];
                data7bitInt.add((fu.i >> 28) & 15);
                data7bitInt.add((fu.i >> 21) & 127);
                data7bitInt.add((fu.i >> 14) & 127);
                data7bitInt.add((fu.i >> 7) & 127);
                data7bitInt.add(fu.i & 127);

            }
            currentDataPointer = currentDataPointer + toSendInThisChunk;
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        
            midiMessages.addEvent(presetMessage, 0);

            currentChunk++;
        }
        data7bitInt.clear();
        data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
        data7bitInt.add(tuningNumber); // which tuning did we just finish
        MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        midiMessages.addEvent(presetMessage, 0);
        waitingToSendTuning = false;
    }
    
    if (waitingToSendOpenString)
    {
        uint16_t currentChunk = 0;
        Array<uint8_t> data7bitInt;
        union uintfUnion fu;
        
        data7bitInt.add(3); // saying it's a preset
        data7bitInt.add(0); // which preset are we saving
        
       

        
        Array<float> data;
        for (int i = 0; i < 4; i++)
        {
            data.add(openStrings[i]);
        }
        
       
        
        uint16_t currentDataPointer = 0;
        uint16_t sizeOfSysexChunk = (64 / 5) - 3;
        int dataToSend = data.size();
        while(currentDataPointer < dataToSend)
        {
            data7bitInt.clear();

            data7bitInt.add(3); // saying it's a tuning
            data7bitInt.add(0); // which tuning are we saving
            
            //data7bitInt.add(currentChunk); // whichChhunk
            uint16_t toSendInThisChunk;
            uint16_t dataRemaining = dataToSend - currentDataPointer;
            if (dataRemaining < sizeOfSysexChunk)
            {
                toSendInThisChunk = dataRemaining;
            }
            else
            {
                toSendInThisChunk = sizeOfSysexChunk;
            }

            for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
            {
                fu.f = data[i];
                data7bitInt.add((fu.i >> 28) & 15);
                data7bitInt.add((fu.i >> 21) & 127);
                data7bitInt.add((fu.i >> 14) & 127);
                data7bitInt.add((fu.i >> 7) & 127);
                data7bitInt.add(fu.i & 127);

            }
            currentDataPointer = currentDataPointer + toSendInThisChunk;
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        
            midiMessages.addEvent(presetMessage, 0);

            currentChunk++;
        }
        data7bitInt.clear();
        data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
        data7bitInt.add(0); // which tuning did we just finish
        MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        midiMessages.addEvent(presetMessage, 0);
        waitingToSendOpenString = false;
    }
    
    if (waitingToSendCopedent)
    {
        Array<float> data;
        for (int i = 0; i < copedentArray.size(); ++i)
        {
            for (auto value : copedentArray.getReference(i))
            {
                data.add(value);
            }
        }
        
        Array<uint8_t> data7bitInt;
        union uintfUnion fu;
        
        uint16_t sizeOfSysexChunk = (64 / 5) - 3;
        int dataToSend = data.size();
        uint16_t currentChunk = 0;
        uint16_t currentDataPointer = 0;
        
        data7bitInt.clear();
        data7bitInt.add(2); // saying it's a copedent
        data7bitInt.add(copedentNumber); // saying which copedent number to store (need this to be a user entered value)
        for (int i = 0; i < copedentName.length(); i++)
        {
            data7bitInt.add((copedentName.toUTF8()[i] & 127)); //printable characters are in the 0-127 range
            
        }
        uint16 remainingBlanks = 14 - copedentName.length();
        for (uint16 i = 0; i < remainingBlanks; i++)
        {
            data7bitInt.add(32);
        }
    
        midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);

        currentChunk++;
        
        
        while(currentDataPointer < dataToSend)
        {
            data7bitInt.clear();

            data7bitInt.add(2); // saying it's a copedent
            data7bitInt.add(copedentNumber); // saying which copedent number to store (need this to be a user entered value)
            //flat7bitInt.add(50 + j);
            
            //data7bitInt.add(currentChunk); // whichChhunk
            uint16_t toSendInThisChunk;
            uint16_t dataRemaining = dataToSend - currentDataPointer;
            if (dataRemaining < sizeOfSysexChunk)
            {
                toSendInThisChunk = dataRemaining;
            }
            else
            {
                toSendInThisChunk = sizeOfSysexChunk;
            }

            for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
            {
                fu.f = data[i];
                data7bitInt.add((fu.i >> 28) & 15);
                data7bitInt.add((fu.i >> 21) & 127);
                data7bitInt.add((fu.i >> 14) & 127);
                data7bitInt.add((fu.i >> 7) & 127);
                data7bitInt.add(fu.i & 127);

            }
            currentDataPointer = currentDataPointer + toSendInThisChunk;
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        
            midiMessages.addEvent(presetMessage, 0);

            currentChunk++;
        }
        data7bitInt.clear();
        data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
        data7bitInt.add(copedentNumber); // which copedent did we just finish
        MidiMessage copedentMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
        midiMessages.addEvent(copedentMessage, 0);
        waitingToSendCopedent = false;
    }
    
    Array<float> resolvedCopedent;
    for (int r = 0; r < MAX_NUM_VOICES; ++r)
    {
        float minBelowZero = 0.0f;
        float maxAboveZero = 0.0f;
        for (int c = 1; c < CopedentColumnNil; ++c)
        {
            if (pedalValues[c]->load() > 0)
            {
                float value = copedentArray.getReference(c)[r];
                if (value < minBelowZero) minBelowZero = value;
                else if (value > maxAboveZero) maxAboveZero = value;
            }
        }
        resolvedCopedent.add(minBelowZero + maxAboveZero);
    }
    //Optimize this away
    for (int i = 0; i < envs.size(); ++i)
    {
        envs[i]->frame();
    }
    for (int i = 0; i < lfos.size(); ++i)
    {
        lfos[i]->frame();
    }
    for (int i = 0; i < oscs.size(); ++i)
    {
        oscs[i]->frame();
    }
    noise->frame();
    for (int i = 0; i < filt.size(); ++i)
    {
        filt[i]->frame();
    }
    for (int i = 0; i < fx.size(); ++i)
    {
        fx[i]->frame();
    }
    output->frame();
    
    /*
    for (auto target : targetMap)
    {
        for (auto voiceTarget : target->targetParameters)
        {
            voiceTarget->tick();
        }
    }
    */
    
    
    int mpe = mpeMode ? 1 : 0;
    int impe = 1-mpe;
    
    for (int s = 0; s < buffer.getNumSamples(); s++)
    {
        tickKnobsToSmooth();
		float parallel = seriesParallelParam->tickNoHooksNoSmoothing();
		float transp = transposeParam->tickNoHooksNoSmoothing();

        for (int i = 0; i < ccParams.size(); ++i)
        {
            ccParams[i]->tickNoHooks();
        }
//        float pitchBends[8];
//        for (int i = 0; i < 8; i++)
//        {
//            pitchBends[i] = pitchBendParams[i]->tickNoHooksNoSmoothing();
//        }

        float globalPitchBend = pitchBendParams[0]->tickNoHooksNoSmoothing();
        
        float samples[2][MAX_NUM_VOICES];
        float outputSamples[2];
        outputSamples[0] = 0.f;
        outputSamples[1] = 0.f;
        
        for (int i = 0; i < envs.size(); ++i)
        {
            envs[i]->tick();
        }
        for (int i = 0; i < lfos.size(); ++i)
        {
            lfos[i]->tick();
        }
        for (int v = 0; v < MAX_NUM_VOICES; v++)
        {
            samples[0][v] = 0.f;
            samples[1][v] = 0.f;//
        }
        for (int v = 0; v < numVoicesActive; ++v)
        {
            float pitchBend = transp + globalPitchBend + pitchBendParams[v+1]->tickNoHooksNoSmoothing();
            float tempNote = (float)tSimplePoly_getPitch(&strings[v*mpe], v*impe);
            //tempNote += resolvedCopedent[v];
            
            //added this check because if there is no active voice "getPitch" returns -1
            if ((tempNote >= 0) && (tempNote < 128))
            {
                //freeze pitch bend data on voices where a note off has happened and we are in the release phase
                if (tSimplePoly_isOn(&strings[v*mpe], v*impe))
                {
                     tempNote += pitchBend;
                     voicePrevBend[v] = pitchBend;
                }
                else
                {
                      tempNote += voicePrevBend[v];
                }
                if ((tempNote >= 0) && (tempNote < 128))
                {
                    int tempNoteIntPart = (int)tempNote;
                    float tempNoteFloatPart = tempNote - (float)tempNoteIntPart;
                    //int tempPitchClassIntPart =tempNoteIntPart % 12;
                    float dev1 = (centsDeviation[tempNoteIntPart] * (1.0f - tempNoteFloatPart));
                    float dev2 =  (centsDeviation[(tempNoteIntPart+1)] * tempNoteFloatPart);
                    float tunedNote = ( dev1  + dev2);
                    voiceNote[v] = tunedNote;
                }
                //DBG("Tuned note" + String(tunedNote));
            }
            //samples[0][v] = 0.f;
            //samples[1][v] = 0.f;
        }
        //per voice inside the tick
        int index = 0;
        for (int i = 0; i < oscs.size();i++)
        {
            if(oscs[i]->getEnabled()) index++;
        }
        oscAmpMult = oscAmpMultArr[index];
        for (int i = 0; i < oscs.size(); ++i)
        {
            oscs[i]->tick(samples);
        }
        //per voice inside
        noise->tick(samples);
        //per voice inside
        filt[0]->tick(samples[0]);
        
        for (int v = 0; v < numVoicesActive; ++v)
        {
            samples[1][v] += samples[0][v]*(1.f-parallel);
        }
        
        filt[1]->tick(samples[1]);
        
        for (int v = 0; v < numVoicesActive; ++v)
        {
            samples[1][v] += samples[0][v]*parallel;
        }
        if (fxPost == nullptr || *fxPost > 0)
        {
            output->tick(samples[1]);
        }
           
        
        
        
        for (int v = 0; v < numVoicesActive; ++v)
        {
            tOversampler_upsample(&os[v], samples[1][v], oversamplerArray);
            for (int i = 0; i < fx.size(); i++) {
                fx[i]->oversample_tick(oversamplerArray, v);
            }
            //hard clip before downsampling to get a little more antialiasing from clipped signal.
            for (int i = 0; i < (OVERSAMPLE); i++)
            {
                oversamplerArray[i] = LEAF_clip(-1.0f, oversamplerArray[i], 1.0f);
            }
            samples[1][v] = tOversampler_downsample(&os[v], oversamplerArray);
        }
    
        if (fxPost == nullptr || *fxPost <= 0)
        {
            output->tick(samples[1]);
        }
        float sampleOutput = 0.0f;
        output->tickLowpass(samples[1]);
        for(int v = 0; v < numVoicesActive; v++)
        {
            sampleOutput += samples[1][v];
        }
        float mastergain = master->tickNoHooks();
        float pedGain = 1.f;
#ifdef ESTEEL
        // this is to clip the gain settings so all the way down on the pedal isn't actually
       // off, it let's a little signal through. Would be more efficient to fix the table to
       // span a better range.
       float volumeSmoothed = ccParams.getUnchecked(12)->get();
       float volIdx = LEAF_clip(47.0f, ((volumeSmoothed * 80.0f) + 47.0f), 127.0f);
       
       //then interpolate the value
       int volIdxInt = (int) volIdx;
       float alpha = volIdx-volIdxInt;
       int volIdxIntPlus = (volIdxInt + 1) & 127;
       float omAlpha = 1.0f - alpha;
       pedGain = volumeAmps128[volIdxInt] * omAlpha;
       pedGain += volumeAmps128[volIdxIntPlus] * alpha;
#endif
        
       outputSamples[0] = sampleOutput * mastergain * pedGain * 0.98f; //drop a little bit to avoid touching clipping
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, s, LEAF_clip(-1.0f, outputSamples[0], 1.0f));
        }
        removeKnobsToSmooth();
    }
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
          setPeakLevel (channel, buffer.getMagnitude (channel, 0, buffer.getNumSamples()));
    }
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        if (stringActivity[i] > 0) stringActivity[i]--;
    }
    scopeDataCollector.process(buffer.getReadPointer(0), (size_t)buffer.getNumSamples());
    if(mute)
    {
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            {
                buffer.setSample(channel, s, 0.0f);
            }
        }
    }
}


//TODO: need to add mapped sources to knobs to smooth array (currently only added by GUI knob twist)
void ElectroAudioProcessor::tickKnobsToSmooth()
{
    for (int i = 0; i < knobsToSmooth.size(); i++)
    {
        SmoothedParameter* knob = knobsToSmooth.getUnchecked(i);
        knob->tick();
        
    }
}


void  ElectroAudioProcessor::removeKnobsToSmooth()
{
    for (int i = 0; i < knobsToSmooth.size(); i++)
    {
        SmoothedParameter* knob = knobsToSmooth.getUnchecked(i);
        if (knob->getRemoveMe())
        {
            knob->setRemoveMe(false);
            knobsToSmooth.remove(i);
        }
    }
    
}
//==============================================================================
bool ElectroAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ElectroAudioProcessor::createEditor()
{
    return new ElectroAudioProcessorEditor (*this, vts);
}

//==============================================================================
void ElectroAudioProcessor::handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    noteOn(midiChannel, midiNoteNumber, velocity);
}

void ElectroAudioProcessor::handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    noteOff(midiChannel, midiNoteNumber, velocity);
}

//==============================================================================
void ElectroAudioProcessor::handleMidiMessage(const MidiMessage& m)
{
    if (m.isNoteOnOrOff())
    {
        keyboardState.processNextMidiEvent(m);
    }
    else
    {
        int channel = m.getChannel();
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

void ElectroAudioProcessor::noteOn(int channel, int key, float velocity)
{
    int i = mpeMode ? channelToStringMap[channel]-1 : 0;
    if (i < 0) return;
    if (!velocity) noteOff(channel, key, velocity);
    else
    {
        int v = tSimplePoly_noteOn(&strings[i], key, velocity * 127.f);
        if (!mpeMode) i = v;
        
        if (v >= 0)
        {
            velocity = ((0.007685533519034f*velocity*127.f) + 0.0239372430f);
            velocity = velocity * velocity;
            key -= midiKeyMin;
            float norm = key / float(midiKeyMax - midiKeyMin);
            midiKeyValues[i] = jlimit(0.f, 1.f, norm);
            velocityValues[i] = velocity;
            float r = leaf.random();
            randomValues[i] = r;
            lastRandomValue = r;
    
            for (auto e : envs) e->noteOn(i, velocity);
            for (auto o : lfos) o->noteOn(i, velocity);
        }
    }
}

void ElectroAudioProcessor::noteOff(int channel, int key, float velocity)
{
    int i = mpeMode ? channelToStringMap[channel]-1 : 0;
    if (i < 0) return;
    
    int v = tSimplePoly_markPendingNoteOff(&strings[i], key);
    
    if (!mpeMode) i = 0;
    //else i = v;
    
    
    
    //If stack_IsNOTEmpty
    if ((v != -1) && (tStack_getSize(&strings[i]->stack) >= numVoicesActive)) {
        if (strings[0]->voices[v][0] == -2)
        {
            
                tSimplePoly_deactivateVoice(&strings[0], v);
                voiceIsSounding[v] = true;
        }
        return;
    }
    if (mpeMode) v = i;
    if (v >= 0)
    {
        for (auto e : envs) e->noteOff(v, velocity);
    }
}

void ElectroAudioProcessor::pitchBend(int channel, int data)
{
    float bend = data * INV_16383;
    if (mpeMode)
    {
        int string = channelToStringMap[channel];
        if (string < 0) return;
        stringActivity[string] = stringActivityTimeout;
        vts.getParameter("PitchBend" + String(string))->setValueNotifyingHost(bend);
    }
    else
    {
        vts.getParameter("PitchBend0")->setValueNotifyingHost(bend);
    }
}

void ElectroAudioProcessor::ctrlInput(int channel, int ctrl, int value)
{
    float v;
    
    // Take all channel CCs outside of MPE mode; only take ch1 in MPE Mode
    if (!mpeMode || channel == stringChannels[0] || channel == 2)
    {
        stringActivity[0] = stringActivityTimeout;
        
        int m = ccNumberToMacroMap[ctrl];
        
        if (0 <= m && m < NUM_GENERIC_MACROS)
        {
            v = value * INV_127;
            vts.getParameter("M" + String(m+1))->setValueNotifyingHost(v);
            ccSources.getUnchecked(m)->setValue(v);
        }
        else if (NUM_GENERIC_MACROS <= m && m < PEDAL_MACRO_ID)
        {
            v = value * INV_127;
            vts.getParameter(cUniqueMacroNames[m-NUM_GENERIC_MACROS])
            ->setValueNotifyingHost(v);
            ccSources.getUnchecked(m)->setValue(v);
        }
    }
}

void ElectroAudioProcessor::sustainOff()
{
    
}

void ElectroAudioProcessor::sustainOn()
{
    
}

void ElectroAudioProcessor::toggleBypass()
{
    
}

void ElectroAudioProcessor::toggleSustain()
{
    
}

//==============================================================================
bool ElectroAudioProcessor::stringIsActive(int string)
{
    if (string == 0) return stringActivity[0] > 0;
    
    int isOn;
    if (mpeMode) isOn = tSimplePoly_isOn(&strings[string-1], 0);
    else isOn = tSimplePoly_isOn(&strings[0], string-1);
    
    return isOn + stringActivity[string] > 0;
}

//==============================================================================
bool ElectroAudioProcessor::getMPEMode()
{
    return mpeMode;
}

void ElectroAudioProcessor::setMPEMode(bool enabled)
{
    mpeMode = enabled;
    tSimplePoly_setNumVoices(&strings[0], mpeMode ? 1 : numVoicesActive);
}

void ElectroAudioProcessor::setNumVoicesActive(int numVoices)
{
    numVoicesActive = numVoices;
    setMPEMode(mpeMode);
}

//==============================================================================
void ElectroAudioProcessor::sendCopedentMidiMessage()
{
    waitingToSendCopedent = true;
}

void ElectroAudioProcessor::sendPresetMidiMessage()
{
   
    waitingToSendPreset = true;
}

void ElectroAudioProcessor::sendTuningMidiMessage(String name, int number)
{
    tuningName = name;
    tuningNumber = number;
    waitingToSendTuning = true;
}

void ElectroAudioProcessor::sendOpenStringMidiMessage()
{
    waitingToSendOpenString = true;
}



//==============================================================================
const juce::String ElectroAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ElectroAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ElectroAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ElectroAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ElectroAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ElectroAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int ElectroAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ElectroAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ElectroAudioProcessor::getProgramName (int index)
{
    return {};
}

void ElectroAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ElectroAudioProcessor::addMappingSource(MappingSourceModel* source)
{
    sourceMap.set(source->name, source);
    DBG("\"" + source->name + "\",");
}

void ElectroAudioProcessor::addMappingTarget(MappingTargetModel* target)
{
    targetMap.set(target->name, target);
}

MappingSourceModel* ElectroAudioProcessor::getMappingSource(const String& name)
{
    return sourceMap.getReference(name);
}

MappingTargetModel* ElectroAudioProcessor::getMappingTarget(const String& name)
{
    return targetMap.getReference(name);
}

//==============================================================================
File ElectroAudioProcessor::loadWaveTables(const String& setName, File& file)
{
    // If we've already loaded this file, just return it
    if (waveTableFiles.contains(file)) return file;
    
    // If this is the root of the wavetable set, keep track of the file
    if (setName == file.getFullPathName()) waveTableFiles.add(file);
//    {
//        waveTableFiles.addIfNotAlreadyThere(file);
//        // And if we've already loaded this, make sure to remove any existing redundant set
//        if (waveTables.contains(setName))
//        {
//            for (auto waveTable : waveTables.getReference(setName))
//            {
//                tWaveTableS_free(&waveTable);
//            }
//            waveTables.remove(setName);
//        }
//    }
    
    // If we have a directory, add all .wav files in that dir to one wavetable set
    if (file.isDirectory())
    {
        for (auto wav : file.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav"))
        {
            loadWaveTables(setName, wav);
        }
    }
    // Read the file, generate a tWaveTableS, and add it to the wavetable set with setName
    else
    {
        auto* reader = formatManager.createReaderFor(file);
        
        Array<AudioBuffer<float>> tableBuffers;
        
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource
                                                                      (reader, true));
            jassert(reader->lengthInSamples%2048 == 0);
            
            for (int i = 0; i < reader->lengthInSamples / 2048; ++i)
            {
                tableBuffers.add(AudioBuffer<float>(reader->numChannels, 2048));
                reader->read(&tableBuffers.getReference(tableBuffers.size()-1), 0,
                             tableBuffers.getReference(tableBuffers.size()-1).getNumSamples(),
                             i*2048, true, true);
            }
            
            readerSource.reset(newSource.release());
        }
        else return File();
        
        Array<tWaveTableS> tables;
        for (int i = 0; i < tableBuffers.size(); ++i)
        {
            tWaveTableS table;
            tWaveTableS_init(&table, tableBuffers.getReference(i).getWritePointer(0), 2048, 14000.f, &leaf);
            tables.add(table);
        }
        
        if (waveTables.contains(setName))
        {
            waveTables.getReference(setName).addArray(tables);
        }
        else
        {
            waveTables.set(setName, tables);
        }
    }
    
    return file;
}

//==============================================================================
void ElectroAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    ValueTree root ("Electrobass");
    
    // Top level settings
    
    root.setProperty("editorScale", editorScale, nullptr);
    root.setProperty("mpeMode", mpeMode, nullptr);
    root.setProperty("numVoices", numVoicesActive, nullptr);
    root.setProperty("midiKeyMin", midiKeyMin, nullptr);
    root.setProperty("midiKeyMax", midiKeyMax, nullptr);
    
    for (int i = 0; i < NUM_MIDI_NOTES; ++i)
    {
        root.setProperty("CentsDev" + String(i+1), centsDeviation[i], nullptr);
    }
    
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        root.setProperty("M" + String(i+1) + "Name", macroNames[i], nullptr);
    }
    
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        root.setProperty("M" + String(i+1) + "CC", macroCCNumbers[i], nullptr);
    }
    
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
    {
        root.setProperty("String" + String(i) + "Ch", stringChannels[i], nullptr);
    }
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        root.setProperty("osc" + String(i+1) + "File",
                         oscs[i]->getWaveTableFile().getFullPathName(), nullptr);
    }
    
    // Audio processor value tree state
    ValueTree state = vts.copyState();
    root.addChild(state, -1, nullptr);
    
    // Copedent
    ValueTree copedent ("Copedent");
    copedent.setProperty("number", copedentNumber, nullptr);
    copedent.setProperty("name", copedentName, nullptr);
    copedent.setProperty("fundamental", copedentFundamental, nullptr);
    for (int c = 0; c < copedentArray.size(); ++c)
    {
        ValueTree column ("c" + String(c));
        for (int r = 0; r < copedentArray.getReference(c).size(); ++r)
        {
            column.setProperty("r" + String(r), copedentArray.getReference(c)[r], nullptr);
        }
        copedent.addChild(column, -1, nullptr);
    }
    root.addChild(copedent, -1, nullptr);
    
    // Mappings
    ValueTree mappings ("Mappings");
    int i = 0;
    for (auto target : targetMap)
    {
        if (MappingSourceModel* source = target->currentSource)
        {
            String scalarName = String();
            if (target->currentScalarSource != nullptr)
            {
                scalarName = target->currentScalarSource->name;
            }
            ValueTree mapping ("m" + String(i++));
            mapping.setProperty("s", source->name, nullptr);
            mapping.setProperty("l", scalarName, nullptr);
            mapping.setProperty("t", target->name, nullptr);
            mapping.setProperty("v", target->end, nullptr);
            mappings.addChild(mapping, -1, nullptr);
        }
    }
    root.addChild(mappings, -1, nullptr);
    
    std::unique_ptr<XmlElement> xml = root.createXml();
    copyXmlToBinary (*xml, destData);
}

void ElectroAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    DBG("Pre set state: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
    
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr)
    {
        // Top level settings
        String presetPath = xml->getStringAttribute("path");
        editorScale = xml->getDoubleAttribute("editorScale", 1.05);
        setMPEMode(xml->getBoolAttribute("mpeMode", false));
        setNumVoicesActive(xml->getIntAttribute("numVoices", 1));
        midiKeyMin = xml->getIntAttribute("midiKeyMin", 21);
        midiKeyMax = xml->getIntAttribute("midiKeyMax", 108);
        for (int i = 0; i < NUM_MIDI_NOTES; ++i)
        {
            centsDeviation[i] = xml->getDoubleAttribute("CentsDev" + String(i+1));
        }
        for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
        {
            macroNames.set(i, xml->getStringAttribute("M" + String(i+1) + "Name",
                                                      "M" + String(i+1)));
        }
        
        for (int i = 1; i <= 127; ++i) ccNumberToMacroMap.set(i, -1);
        for (int i = 0; i < NUM_MACROS+1; ++i)
        {
            macroCCNumbers[i] = xml->getIntAttribute("M" + String(i+1) + "CC", i+1);
            ccNumberToMacroMap.set(macroCCNumbers[i], i);
        }
        
        for (int i = 1; i <= 16; ++i) channelToStringMap.set(i, -1);
        for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
        {
            stringChannels[i] = xml->getIntAttribute("String" + String(i) + "Ch", i+1);
            channelToStringMap.set(stringChannels[i], i);
        }
        
        for (int i = 0; i < NUM_OSCS; ++i)
        {
            String filePath = xml->getStringAttribute("osc" + String(i+1) + "File");
            if(filePath.isEmpty())
            {
                continue;
            }
            File wav (filePath);
            
            if (wav.exists())
            {
                DBG("Pre osc set state: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
                oscs[i]->setWaveTables(wav);
                DBG("Post osc set state: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
            } else
            {
                DBG(wav.getFileName());
                File f = File::getSpecialLocation(File::globalApplicationsDirectory).getFullPathName() + "/Electrobass/" + wav.getFileName();
                DBG(f.getFullPathName());
                if (f.exists())
                {
                    DBG("fileesxist");
                    oscs[i]->setWaveTables(f);
                }
            }
        }
        // Audio processor value tree state
        if (XmlElement* state = xml->getChildByName(vts.state.getType()))
            vts.replaceState (juce::ValueTree::fromXml (*state));
        DBG("Effect1 PAram2 " + String(vts.getRawParameterValue("Effect1 Param2")->load(std::memory_order_relaxed)));
        for (int v = 0; v < numVoicesActive; v++)
        {
            for (int i = 0; i < NUM_OSCS; i++)
            {
                oscs[i]->loadAll(v);
            }
            for (int i = 0; i < NUM_ENVS; i++)
            {
                envs[i]->loadAll(v);
            }
            noise->loadAll(v);
            for (int i = 0; i < NUM_FILT; i++)
            {
                filt[i]->loadAll(v);
            }
            for (int i = 0; i < NUM_LFOS; i++)
            {
                lfos[i]->loadAll(v);
            }
            for (int i = 0; i < NUM_FX; i++)
            {
                fx[i]->loadAll(v);
            }
        }
        
        // Copedent
        if (XmlElement* copedent = xml->getChildByName("Copedent"))
        {
            copedentNumber = copedent->getIntAttribute("number");
            copedentName = copedent->getStringAttribute("name");
            copedentFundamental = copedent->getDoubleAttribute("fundamental");
            for (int c = 0; c < copedentArray.size(); ++c)
            {
                XmlElement* column = copedent->getChildByName("c" + String(c));
                for (int r = 0; r < copedentArray.getReference(c).size(); ++r)
                {
                    float value = column->getDoubleAttribute("r" + String(r));
                    copedentArray.getReference(c).set(r, value);
                }
            }
        }

		for (auto target : targetMap)
		{
            if (target->currentSource != nullptr)
            {
                target->removeMapping(true);
            }
		}
        
        // Mappings
        initialMappings.clear();
        if (XmlElement* mappings = xml->getChildByName("Mappings"))
        {
            for (auto child : mappings->getChildIterator())
            {
                Mapping m;
                m.sourceName = child->getStringAttribute("s");
                m.scalarName = child->getStringAttribute("l");
                m.targetName = child->getStringAttribute("t");
                m.value = child->getDoubleAttribute("v");
                initialMappings.add(m);
            }
        }
        else
        {
			// A couple of default mappings that will be used if nothing has been saved
            //TODO: this seems wrong now that we changed envelope 1 to always be amp mapping
			Mapping defaultFilter1Cutoff;
			defaultFilter1Cutoff.sourceName = "Envelope3";
			defaultFilter1Cutoff.targetName = "Filter1 Cutoff T3";
			defaultFilter1Cutoff.value = 24.f;

			Mapping defaultOutputAmp;
			defaultOutputAmp.sourceName = "Envelope4";
			defaultOutputAmp.targetName = "Output Amp T3";
			defaultOutputAmp.value = 1.f;

			initialMappings.add(defaultFilter1Cutoff);
			initialMappings.add(defaultOutputAmp);
        }

		if (!initialMappings.isEmpty()) // First prepareToPlay
		{
			for (Mapping m : initialMappings)
			{
				targetMap[m.targetName]->setMapping(sourceMap[m.sourceName], m.value, false);
				targetMap[m.targetName]->setMappingScalar(sourceMap[m.scalarName], false);
			}
			initialMappings.clear();
		}
    }
    
    if (ElectroAudioProcessorEditor* editor = dynamic_cast<ElectroAudioProcessorEditor*>(getActiveEditor()))
    {
        editor->update();
    }
    
    DBG("Post set state: " + String(leaf.allocCount) + " " + String(leaf.freeCount));
}


void ElectroAudioProcessor::setPeakLevel (int channelIndex, float peakLevel)
{
   if (! juce::isPositiveAndBelow (channelIndex, m_peakLevels.size())) return;

   m_peakLevels[channelIndex].store (std::max (peakLevel, m_peakLevels[channelIndex].load()));
}

float ElectroAudioProcessor::getPeakLevel (int channelIndex)
{
   if (! juce::isPositiveAndBelow (channelIndex, m_peakLevels.size())) return 0.0f;

   return m_peakLevels[channelIndex].exchange (0.0f);
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ElectroAudioProcessor();
}

//PARAMS//
//Master: 0
//M1: 1
//M2: 2
//M3: 3
//M4: 4
//M5: 5
//M6: 6
//M7: 7
//M8: 8
//M9: 9
//M10: 10
//M11: 11
//M12: 12
//M13: 13
//M14: 14
//M15: 15
//M16: 16
//PitchBend0: 17
//PitchBend1: 18
//PitchBend2: 19
//PitchBend3: 20
//PitchBend4: 21
//PitchBend5: 22
//PitchBend6: 23
//PitchBend7: 24
//PitchBend8: 25
//PitchBend9: 26
//PitchBend10: 27
//PitchBend11: 28
//PitchBend12: 29
//Osc1: 30
//Osc1 Pitch: 31
//Osc1 Fine: 32
//Osc1 Shape: 33
//Osc1 Amp: 34
//Osc1 ShapeSet: 35
//Osc1 FilterSend: 36
//Osc2: 37
//Osc2 Pitch: 38
//Osc2 Fine: 39
//Osc2 Shape: 40
//Osc2 Amp: 41
//Osc2 ShapeSet: 42
//Osc2 FilterSend: 43
//Osc3: 44
//Osc3 Pitch: 45
//Osc3 Fine: 46
//Osc3 Shape: 47
//Osc3 Amp: 48
//Osc3 ShapeSet: 49
//Osc3 FilterSend: 50
//Filter1: 51
//Filter1 Type: 52
//Filter1 Cutoff: 53
//Filter1 Resonance: 54
//Filter1 KeyFollow: 55
//Filter2: 56
//Filter2 Type: 57
//Filter2 Cutoff: 58
//Filter2 Resonance: 59
//Filter2 KeyFollow: 60
//Filter Series-Parallel Mix: 61
//Envelope1 Attack: 62
//Envelope1 Decay: 63
//Envelope1 Sustain: 64
//Envelope1 Release: 65
//Envelope1 Leak: 66
//Envelope1 Velocity: 67
//Envelope2 Attack: 68
//Envelope2 Decay: 69
//Envelope2 Sustain: 70
//Envelope2 Release: 71
//Envelope2 Leak: 72
//Envelope2 Velocity: 73
//Envelope3 Attack: 74
//Envelope3 Decay: 75
//Envelope3 Sustain: 76
//Envelope3 Release: 77
//Envelope3 Leak: 78
//Envelope3 Velocity: 79
//Envelope4 Attack: 80
//Envelope4 Decay: 81
//Envelope4 Sustain: 82
//Envelope4 Release: 83
//Envelope4 Leak: 84
//Envelope4 Velocity: 85
//LFO1 Rate: 86
//LFO1 Shape: 87
//LFO1 Sync Phase: 88
//LFO1 ShapeSet: 89
//LFO1 Sync: 90
//LFO2 Rate: 91
//LFO2 Shape: 92
//LFO2 Sync Phase: 93
//LFO2 ShapeSet: 94
//LFO2 Sync: 95
//LFO3 Rate: 96
//LFO3 Shape: 97
//LFO3 Sync Phase: 98
//LFO3 ShapeSet: 99
//LFO3 Sync: 100
//LFO4 Rate: 101
//LFO4 Shape: 102
//LFO4 Sync Phase: 103
//LFO4 ShapeSet: 104
//LFO4 Sync: 105
//Output Amp: 106
//Output Pan: 107

//SOURCE//
//M1: 0
//M2: 1
//M3: 2
//M4: 3
//M5: 4
//M6: 5
//M7: 6
//M8: 7
//M9: 8
//M10: 9
//M11: 10
//M12: 11
//M13: 12
//M14: 13
//M15: 14
//M16: 15
//Envelope1: 16
//Envelope2: 17
//Envelope3: 18
//Envelope4: 19
//LFO1: 20
//LFO2: 21
//LFO3: 22
//LFO4: 23

//Mapping looks like:
//SourceID, TargetParamID, TargetIndex(0-2), Scale
