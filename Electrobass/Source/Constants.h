/*
  ==============================================================================

    Constants.h
    Created: 9 Mar 2021 11:58:01am
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <float.h>
#define EBASS 1
#define EDITOR_WIDTH 900.0f
#define EDITOR_HEIGHT 700.0f
#define NUM_GENERIC_MACROS 8
#define NUM_MIDI_NOTES 128
#define NUM_UNIQUE_MACROS 15
#define NUM_MACROS (NUM_GENERIC_MACROS + NUM_UNIQUE_MACROS)
#define PEDAL_MACRO_ID (NUM_MACROS-1)

#define NUM_STRINGS 12
#define MAX_NUM_VOICES 12
#define NUM_CHANNELS (MAX_NUM_VOICES+1)

#define NUM_OSCS 3
#define INV_NUM_OSCS 0.333333f
#define NUM_FILT 2
#define NUM_ENVS 4
#define NUM_LFOS 4
#define NUM_FX 4
#define OVERSAMPLE 2
#define EXP_BUFFER_SIZE 2048
#define DECAY_EXP_BUFFER_SIZE 2048

#define MAX_NUM_UNIQUE_SKEWS 10

#define INV_127 0.007874015748031f
#define INV_4095 0.0002442002442f
#define INV_16383 0.000061038881768f
#define PI_DIV_2 1.570796326794897f

const float volumeAmps128[128] =
{
    0.000562, 0.000569, 0.000577, 0.000580, 0.000587, 0.000601, 0.000622, 0.000650, 0.000676,
    0.000699, 0.000720, 0.000739, 0.000753, 0.000766, 0.000791, 0.000826, 0.000872, 0.000912,
    0.000953, 0.001012, 0.001091, 0.001188, 0.001270, 0.001360, 0.001465, 0.001586, 0.001717,
    0.001829, 0.001963, 0.002118, 0.002295, 0.002469, 0.002636, 0.002834, 0.003063, 0.003322,
    0.003496, 0.003750, 0.004143, 0.004675, 0.005342, 0.005880, 0.006473, 0.007122, 0.007827,
    0.008516, 0.009167, 0.009968, 0.010916, 0.012014, 0.012944, 0.013977, 0.015352, 0.017070,
    0.019130, 0.020965, 0.022847, 0.024823, 0.026891, 0.028835, 0.030496, 0.033044, 0.036478,
    0.040799, 0.045093, 0.049150, 0.053819, 0.059097, 0.064986, 0.070712, 0.076315, 0.081930,
    0.087560, 0.093117, 0.098283, 0.104249, 0.111012, 0.118575, 0.124879, 0.131163, 0.141721,
    0.156554, 0.175663, 0.195870, 0.213414, 0.228730, 0.241817, 0.252675, 0.264038, 0.276776,
    0.290871, 0.306323, 0.322794, 0.338528, 0.353711, 0.368343, 0.382424, 0.393015, 0.406556,
    0.426763, 0.453639, 0.487182, 0.522242, 0.550876, 0.573000, 0.588613, 0.598943, 0.613145,
    0.628104, 0.643820, 0.660293, 0.676658, 0.692845, 0.709881, 0.727766, 0.746500, 0.764505,
    0.782949, 0.802346, 0.822696, 0.844189, 0.867268, 0.886360, 0.901464, 0.912581, 0.921606,
    0.932834, 0.944061
};

//==============================================================================
static const std::vector<std::string> cUniqueMacroNames = {
    "A",
    "B",
    "X",
    "Y",
    "Ped",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "K1",
    "K2",
    "K3",
    "K4",
    "K5"
};

//==============================================================================

typedef enum _OscParam
{
    OscPitch = 0,
    OscFine,
    OscFreq,
    OscShape,
    OscAmp,
    OscHarm,
    OscParamNil
} OscParam;
static const StringArray cOscParams = {
    "Pitch",
    "Fine",
    "Freq",
    "Shape",
    "Amp",
    "Harmonics"
};
static const std::vector<std::vector<float>> vOscInit = {
    { -24.0f, 24.0f, 0.0f, 0.0f }, //Pitch
    { -100.f, 100.f, 0.0f, 0.0f }, //Fine
    { -2000.f, 2000.f, 0.0f, 0.0f }, //Freq
    { 0.0f, 1.0f, 0.0f, 0.5f },  //Shape
    { 0.0f, 1.0f, 1.0f, 0.5f },  //Amp
    { -16, 16, 0.0f, 0.0f} //harmonics
};

typedef enum _OscShapeSet
{
    SawPulseOscShapeSet = 0,
    SineTriOscShapeSet,
    SawOscShapeSet,
    PulseOscShapeSet,
    SineOscShapeSet,
    TriOscShapeSet,
    UserOscShapeSet,
    OscShapeSetNil
} OscShapeSet;
static const StringArray oscShapeSetNames = {
    "Saw-Square",
    "Sine-Tri",
    "Saw",
    "Pulse",
    "Sine",
    "Tri",
    "Select file..."
};

//==============================================================================

typedef enum _LowFreqParam
{
    LowFreqRate = 0,
    LowFreqShape,
    LowFreqPhase,
    LowFreqParamNil
} LowFreqParam;
static const StringArray cLowFreqParams = {
    "Rate",
    "Shape",
    "Phase"
};
static const std::vector<std::vector<float>> vLowFreqInit = {
    { 0.0f, 30.f, 0.5f, 2.f },  //Rate
    { 0.0f, 1.0f, 0.0f, 0.5f },  //Shape
    { 0.0f, 1.0f, 0.0f, 0.5f } // Phase Offset
};

typedef enum _LFOShapeSet
{
    SineTriLFOShapeSet = 0,
    SawPulseLFOShapeSet,
    SineLFOShapeSet,
    TriLFOShapeSet,
    SawLFOShapeSet,
    PulseLFOShapeSet,
    LFOShapeSetNil
} LFOShapeSet;
static const StringArray lfoShapeSetNames = {
    "Sine-Tri",
    "Saw-Square",
    "Sine",
    "Tri",
    "Saw",
    "Pulse"
};

//==============================================================================

typedef enum _NoiseParam
{
    NoiseTilt= 0,
    NoiseGain,
    NoiseFreq,
    NoiseAmp,
} NoiseParam;
static const StringArray cNoiseParams = {
    "Tilt",
    "PeakGain",
    "PeakFreq",
    "Amp"
};
static const std::vector<std::vector<float>> vNoiseInit = {
    { 0.0f, 1.f, 0.5f, 0.5f },   //tilt
    { 0.0f, 1.f, 0.5f, 0.5f },  //Gain
    { 0.0f, 1.f, 0.5f, 0.5f },   //Freq
    { 0.0f, 2.0f, 0.f, 1.f }   //Amp
};

//==============================================================================

typedef enum _FilterParam
{
    FilterCutoff = 0,
    FilterGain,
    FilterResonance,
    FilterKeyFollow,
    FilterParamNil
} FilterParam;
static const StringArray cFilterParams = {
    "Cutoff",
    "Gain",
    "Resonance",
    "KeyFollow"
};
static const std::vector<std::vector<float>> vFilterInit = {
    { 0.0f, 127.f, 72.f, 63.5f },   //Cutoff
    { 0.0f, 1.f, 0.5f, 0.5f }, //Gain
    { 0.01f, 10.f, 0.5f, 0.5f },   //Resonance
    { 0.0f, 1.f, 0.5f, 0.5f }  //KeyFollow
    
};

typedef enum _FilterType
{
    LowpassFilter = 0,
    HighpassFilter,
    BandpassFilter,
    DiodeLowpassFilter,
    VZPeakFilter,
    VZLowshelfFilter,
    VZHighshelfFilter,
    VZBandrejectFilter,
    LadderLowpassFilter,
    FilterTypeNil
} FilterType;

static const StringArray filterTypeNames = {
    "Lowpass",
    "Highpass",
    "Bandpass",
    "DiodeLowpass",
    "Peak",
    "Lowshelf",
    "Highshelf",
    "Notch",
    "LadderLowpass"
};

//==============================================================================

typedef enum _EnvelopeParam
{
    EnvelopeAttack = 0,
    EnvelopeDecay,
    EnvelopeSustain,
    EnvelopeRelease,
    EnvelopeLeak,
    EnvelopeParamNil
} EnvelopeParam;
static const StringArray cEnvelopeParams = {
    "Attack",
    "Decay",
    "Sustain",
    "Release",
    "Leak"
};
static const std::vector<std::vector<float>> vEnvelopeInit = {
    { 0.0f, 20000.0f, 7.f, 4000.f },   //Attack
    { 0.0f, 20000.0f, 1000.f, 4000.f },  //Decay
    { 0.0f, 1.f, 0.5f, 0.5f },   //Sustain
    { 0.0f, 20000.0f, 20.f, 4000.f },   //Release
    { 0.0f, 1.0f, 0.f, 0.5f },   //Leak
};

//==============================================================================

typedef enum _OutputParam
{
    OutputAmp,
    OutputTone,
    OutputParamNil
} OutputParam;
static const StringArray cOutputParams = {
    "Amp",
    "Tone"
};
static const std::vector<std::vector<float>> vOutputInit = {
    { 0.0f, 1.0f, 0.0f, 0.5f },   //Amp
    { 0.0f, 1.0, 1.0f, 0.5f },   //Tone
};


//==============================================================================

typedef enum _CopedentColumn
{
    CopedentString = 0,
    CopedentLKL,
    CopedentLKV,
    CopedentLKR,
    CopedentP1,
    CopedentP2,
    CopedentP3,
    CopedentP4,
    CopedentP5,
    CopedentRKL,
    CopedentRKR,
    CopedentColumnNil
} CopedentColumn;

static const std::vector<std::string> cCopedentColumnNames = {
    "Strings",
    "LKL",
    "LKV",
    "LKR",
    "P1",
    "P2",
    "P3",
    "P4",
    "P5",
    "RKL",
    "RKR"
};

static const std::vector<std::vector<float>> cCopedentArrayInit = {
    { 66.f, 63.f, 56.f, 52.f, 47.f, 44.f, 42.f, 40.f, 35.f, 32.f, 28.f, 23.f },
    
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
};



typedef enum _FXParam
{
    Param1 = 0,
    Param2,
    Param3,
    Param4,
    Param5,
    Mix,
    PostGain,
    ParamNil
} FXParam;
static const StringArray cFXParams = {
    "Param1",
    "Param2",
    "Param3",
    "Param4",
    "Param5",
    "Mix",
    "PostGain"
};
static const std::vector<std::vector<float>> vFXInit = {
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 1.0f, 0.5f },   // mix
    { 0.0f, 1.0f, 0.5f, 0.5f }   // postgain
};

typedef enum _FXType
{
    None = 0,
    Softclip,
    Hardclip,
    ABSaturator,
    Tanh,
    Shaper,
    Compressor,
    Chorus,
    Bitcrush,
    TiltFilter,
    Wavefolder,
    LpFilter,
    HpFilter,
    BpFilter,
    DLFilter,
    VZPFilter,
    VZLFilter,
    VZHFilter,
    VZBFilter,
    LLFilter,
    FXTypeNil
} FXType;

static const StringArray FXTypeNames = {
    "None",
    "Softclip",
    "Hardclip",
    "ABSaturator",
    "Tanh",
    "Shaper",
    "Compressor",
    "Chorus",
    "Bitcrush",
    "TiltFilter",
    "Wavefolder",
    "Lowpass",
    "Highpass",
    "Bandpass",
    "DiodeLowpass",
    "Peak",
    "Lowshelf",
    "Highshelf",
    "Notch",
    "LadderLowpass"
};

static const std::vector<StringArray> FXParamNames = {
    {"","","","","" },
    {"Drive","Offset","Shape","","" },
    {"Drive","Offset","Shape","","" },
    {"Drive","Offset","","",""},
    {"Drive","Offset","","",""},
    {"Gain","Offset","Drive","",""},
    {"Threshold","Ratio","Makeup","Attack","Release"},
    {"Delay","Depth","Speed1","Speed2",""},
    {"Gain","Crush","Decimate","Round","Operation"},
    {"Tilt","PeakFreq","PeakQ","PeakGain",""},
    {"Drive","Offset","FB","FF","Depth"},
    {"Cutoff", "", "Resonance" ,"",""},
    {"Cutoff", "", "Resonance" ,"",""},
    {"Cutoff", "", "Resonance" ,"",""},
    {"Cutoff", "", "Resonance" ,"",""},
    {"Cutoff", "Gain", "Resonance" ,"",""},
    {"Cutoff", "Gain", "Resonance" ,"",""},
    {"Cutoff", "Gain", "Resonance" ,"",""},
    {"Cutoff", "", "Resonance" ,"",""},
    {"Cutoff", "", "Resonance" ,"",""}
};

static const std::vector<std::vector<float>> FXParamDefaults = {
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.3f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.5f, 0.4f, 0.4f, 0.05f, 0.25f},
    {0.65f, 0.65f, 0.1f, 0.25f, 0.0f},
    {0.5f, 0.5f, 0.2f, 0.8f, 0.25f},
    {0.5f, 0.5f, 0.5f, 0.5f, 0.0f},
    {0.4f, 0.5f, 0.6f, 0.4f, 0.5f},
    {0.75f, 0.5f, 0.5f, 0.0f, 0.0f},
    {0.35f, 0.5f, 0.5f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f},
    {0.65f, 0.5f, 0.5f, 0.0f, 0.0f},
    {0.5f, 0.75f, 0.5f, 0.0f, 0.0f},
    {0.45f, 0.65f, 0.4f, 0.0f, 0.0f},
    {0.5f, 0.65f, 0.5f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f},
    {0.6f, 0.5f, 0.55f, 0.0f, 0.0f}
};

//==============================================================================
static const std::vector<String> paramDestOrder
{
    "Master",
    "M1",
    "M2",
    "M3",
    "M4",
    "M5",
    "M6",
    "M7",
    "M8",
    "A",
    "B",
    "X",
    "Y",
    "Ped",
    "PitchBendRange",
    "Noise",
    "Noise Tilt",
    "Noise PeakGain",
    "Noise PeakFreq",
    "Noise Amp",
    "Noise FilterSend",
    "Osc1",
    "Osc1 Pitch",
    "Osc1 Fine",
    "Osc1 Freq",
    "Osc1 Shape",
    "Osc1 Amp",
    "Osc1 Harmonics",
    "Osc1 isHarmonic",
    "Osc1 isStepped",
    "Osc1 isSync",
    "Osc1 syncType",
    "Osc1 ShapeSet",
    "Osc1 FilterSend",
    "Osc2",
    "Osc2 Pitch",
    "Osc2 Fine",
    "Osc2 Freq",
    "Osc2 Shape",
    "Osc2 Amp",
    "Osc2 Harmonics",
    "Osc2 isHarmonic",
    "Osc2 isStepped",
    "Osc2 isSync",
    "Osc2 syncType",
    "Osc2 ShapeSet",
    "Osc2 FilterSend",
    "Osc3",
    "Osc3 Pitch",
    "Osc3 Fine",
    "Osc3 Freq",
    "Osc3 Shape",
    "Osc3 Amp",
    "Osc3 Harmonics",
    "Osc3 isHarmonic",
    "Osc3 isStepped",
    "Osc3 isSync",
    "Osc3 syncType",
    "Osc3 ShapeSet",
    "Osc3 FilterSend",
    "Effect1 FXType",
    "Effect1 Param1",
    "Effect1 Param2",
    "Effect1 Param3",
    "Effect1 Param4",
    "Effect1 Param5",
    "Effect1 Mix",
    "Effect1 PostGain",
    "Effect2 FXType",
    "Effect2 Param1",
    "Effect2 Param2",
    "Effect2 Param3",
    "Effect2 Param4",
    "Effect2 Param5",
    "Effect2 Mix",
    "Effect2 PostGain",
    "Effect3 FXType",
    "Effect3 Param1",
    "Effect3 Param2",
    "Effect3 Param3",
    "Effect3 Param4",
    "Effect3 Param5",
    "Effect3 Mix",
    "Effect3 PostGain",
    "Effect4 FXType",
    "Effect4 Param1",
    "Effect4 Param2",
    "Effect4 Param3",
    "Effect4 Param4",
    "Effect4 Param5",
    "Effect4 Mix",
    "Effect4 PostGain",
    "Filter1",
    "Filter1 Type",
    "Filter1 Cutoff",
    "Filter1 Gain",
    "Filter1 Resonance",
    "Filter1 KeyFollow",
    "Filter2",
    "Filter2 Type",
    "Filter2 Cutoff",
    "Filter2 Gain",
    "Filter2 Resonance",
    "Filter2 KeyFollow",
    "Filter Series-Parallel Mix",
    "Envelope1 Attack",
    "Envelope1 Decay",
    "Envelope1 Sustain",
    "Envelope1 Release",
    "Envelope1 Leak",
    "Envelope1 Velocity",
    "Envelope2 Attack",
    "Envelope2 Decay",
    "Envelope2 Sustain",
    "Envelope2 Release",
    "Envelope2 Leak",
    "Envelope2 Velocity",
    "Envelope3 Attack",
    "Envelope3 Decay",
    "Envelope3 Sustain",
    "Envelope3 Release",
    "Envelope3 Leak",
    "Envelope3 Velocity",
    "Envelope4 Attack",
    "Envelope4 Decay",
    "Envelope4 Sustain",
    "Envelope4 Release",
    "Envelope4 Leak",
    "Envelope4 Velocity",
    "LFO1 Rate",
    "LFO1 Shape",
    "LFO1 Phase",
    "LFO1 ShapeSet",
    "LFO1 Sync",
    "LFO2 Rate",
    "LFO2 Shape",
    "LFO2 Phase",
    "LFO2 ShapeSet",
    "LFO2 Sync",
    "LFO3 Rate",
    "LFO3 Shape",
    "LFO3 Phase",
    "LFO3 ShapeSet",
    "LFO3 Sync",
    "LFO4 Rate",
    "LFO4 Shape",
    "LFO4 Phase",
    "LFO4 ShapeSet",
    "LFO4 Sync",
    "Output Amp",
    "Output Tone",
    "FX Order"
};
#define OSC_SOURCE_OFFSET 0
#define NOISE_SOURCE_OFFSET 3
#define MACRO_SOURCE_OFFSET 4
#define CTRL_SOURCE_OFFSET 12
#define MIDI_KEY_SOURCE_OFFSET 17
#define VELOCITY_SOURCE_OFFSET 18
#define RANDOM_SOURCE_OFFSET 19
#define ENV_SOURCE_OFFSET 20
#define LFO_SOURCE_OFFSET 24
#define PEDAL_SOURCE_OFFSET 28
static const std::vector<String> paramSourceOrder
{
    "Osc1",
    "Osc2",
    "Osc3",
    "Noise",
    "M1",
    "M2",
    "M3",
    "M4",
    "M5",
    "M6",
    "M7",
    "M8",
    "A",
    "B",
    "X",
    "Y",
    "Ped",
    "MIDI Key In",
    "Velocity In",
    "Random on Attack",
    "Envelope1",
    "Envelope2",
    "Envelope3",
    "Envelope4",
    "LFO1",
    "LFO2",
    "LFO3",
    "LFO4",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "cLKL",
    "cLKV",
    "cLKR",
    "cRKL",
    "cRKR"
};
//enum ParamNames
//{
//    Master,
//    M1,
//    M2,
//    M3,
//    M4,
//    M5,
//    M6,
//    M7,
//    M8,
//    M9,
//    M10,
//    M11,
//    M12,
//    M13,
//    M14,
//    M15,
//    M16,
//    A,
//    B,
//    X,
//    Y,
//    Ped,
//    Transpose,
//    PitchBend0,
//    PitchBend1,
//    PitchBend2,
//    PitchBend3,
//    PitchBend4,
//    PitchBend5,
//    PitchBend6,
//    PitchBend7,
//    PitchBend8,
//    PitchBendRangeUp,
//    PitchBendRangeDown,
//    Noise,
//    NoiseTilt,
//    NoisePeakGain,
//    NoisePeakFreq,
//    NoiseAmp,
//    NoiseFilterSend,
//    Osc1,
//    Osc1Pitch,
//    Osc1Fine,
//    Osc1Freq,
//    Osc1Shape,
//    Osc1Amp,
//    Osc1isHarmonic,
//    Osc1isStepped,
//    Osc1ShapeSet,
//    Osc1FilterSend,
//    Osc2,
//    Osc2Pitch,
//    Osc2Fine,
//    Osc2Freq,
//    Osc2Shape,
//    Osc2Amp,
//    Osc2isHarmonic,
//    Osc2isStepped,
//    Osc2ShapeSet,
//    Osc2FilterSend,
//    Osc3,
//    Osc3Pitch,
//    Osc3Fine,
//    Osc3Freq,
//    Osc3Shape,
//    Osc3Amp,
//    Osc3isHarmonic,
//    Osc3isStepped,
//    Osc3ShapeSet,
//    Osc3FilterSend,
//    Effect1FXType,
//    Effect1Param1,
//    Effect1Param2,
//    Effect1Param3,
//    Effect1Param4,
//    Effect1Param5,
//    Effect1Mix,
//    Effect2FXType,
//    Effect2Param1,
//    Effect2Param2,
//    Effect2Param3,
//    Effect2Param4,
//    Effect2Param5,
//    Effect2Mix,
//    Effect3FXType,
//    Effect3Param1,
//    Effect3Param2,
//    Effect3Param3,
//    Effect3Param4,
//    Effect3Param5,
//    Effect3Mix,
//    Effect4FXType,
//    Effect4Param1,
//    Effect4Param2,
//    Effect4Param3,
//    Effect4Param4,
//    Effect4Param5,
//    Effect4Mix,
//    Filter1,
//    Filter1Type,
//    Filter1Cutoff,
//    Filter1Gain,
//    Filter1Resonance,
//    Filter1KeyFollow,
//    Filter2,
//    Filter2Type,
//    Filter2Cutoff,
//    Filter2Gain,
//    Filter2Resonance,
//    Filter2KeyFollow,
//    FilterSeriesParallelMix,
//    Envelope1Attack,
//    Envelope1Decay,
//    Envelope1Sustain,
//    Envelope1Release,
//    Envelope1Leak,
//    Envelope1Velocity,
//    Envelope2Attack,
//    Envelope2Decay,
//    Envelope2Sustain,
//    Envelope2Release,
//    Envelope2Leak,
//    Envelope2Velocity,
//    Envelope3Attack,
//    Envelope3Decay,
//    Envelope3Sustain,
//    Envelope3Release,
//    Envelope3Leak,
//    Envelope3Velocity,
//    Envelope4Attack,
//    Envelope4Decay,
//    Envelope4Sustain,
//    Envelope4Release,
//    Envelope4Leak,
//    Envelope4Velocity,
//    LFO1Rate,
//    LFO1Shape,
//    LFO1Phase,
//    LFO1ShapeSet,
//    LFO1Sync,
//    LFO2Rate,
//    LFO2Shape,
//    LFO2Phase,
//    LFO2ShapeSet,
//    LFO2Sync,
//    LFO3Rate,
//    LFO3Shape,
//    LFO3Phase,
//    LFO3ShapeSet,
//    LFO3Sync,
//    LFO4Rate,
//    LFO4Shape,
//    LFO4Phase,
//    LFO4ShapeSet,
//    LFO4Sync,
//    OutputAmp,
//};
