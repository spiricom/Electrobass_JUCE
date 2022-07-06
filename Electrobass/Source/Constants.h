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
#define EDITOR_WIDTH 900.0f
#define EDITOR_HEIGHT 700.0f
#define NUM_GENERIC_MACROS 16
#define NUM_UNIQUE_MACROS 5
#define NUM_MACROS (NUM_GENERIC_MACROS + NUM_UNIQUE_MACROS)
#define PEDAL_MACRO_ID (NUM_MACROS-1)

#define NUM_STRINGS 8
#define MAX_NUM_VOICES 8
#define NUM_CHANNELS (MAX_NUM_VOICES+1)

#define NUM_OSCS 3
#define INV_NUM_OSCS 0.333333f
#define NUM_FILT 2
#define NUM_ENVS 4
#define NUM_LFOS 4
#define NUM_FX 4
#define OVERSAMPLE 4
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
    "Ped"
};

//==============================================================================

typedef enum _OscParam
{
    OscPitch = 0,
    OscFine,
    OscFreq,
    OscShape,
    OscAmp,
    OscParamNil
} OscParam;
static const StringArray cOscParams = {
    "Pitch",
    "Fine",
    "Freq",
    "Shape",
    "Amp",
};
static const std::vector<std::vector<float>> vOscInit = {
    { -24.0f, 24.0f, 0.0f, 0.0f }, //Pitch
    { -100.f, 100.f, 0.0f, 0.0f }, //Fine
    { -2000.f, 2000.f, 0.0f, 0.0f }, //Freq
    { 0.0f, 1.0f, 0.0f, 0.5f },  //Shape
    { 0.0f, 2.0f, 1.0f, 1.0f },  //Amp
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
    LowFreqSyncPhase,
    LowFreqParamNil
} LowFreqParam;
static const StringArray cLowFreqParams = {
    "Rate",
    "Shape",
    "Sync Phase"
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
    OutputParamNil
} OutputParam;
static const StringArray cOutputParams = {
    "Amp",
};
static const std::vector<std::vector<float>> vOutputInit = {
    { 0.0f, 2.0f, 0.0f, 1.0f },   //Amp
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
    ParamNil
} FXParam;
static const StringArray cFXParams = {
    "Param1",
    "Param2",
    "Param3",
    "Param4",
    "Param5",
    "Mix"
};
static const std::vector<std::vector<float>> vFXInit = {
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 0.0f, 0.5f },   //
    { 0.0f, 1.0f, 1.0f, 0.5f },   // mix
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
    "Wavefolder"
};

static const std::vector<StringArray> FXParamNames = {
    {"","","","","" },
    {"Drive","Offset","Shape","PostGain","" },
    {"Drive","Offset","Shape","PostGain","" },
    {"Drive","Offset","Shape","PostGain",""},
    {"Drive","Offset","Shape","PostGain",""},
    {"Gain","Offset","Drive","PostGain",""},
    {"Threshold","Ratio","Makeup","Attack","Release"},
    {"Delay","Depth","Speed1","Speed2",""},
    {"Gain","Quality","Sampling Ratio","Round","Operation"},
    {"Tilt","PeakFreq","PeakQ","PeakGain","PostGain"},
    {"Drive","Offset","PostGain","",""}
};

static const std::vector<std::vector<float>> FXParamDefaults = {
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.1f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.1f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.3f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.2f, 0.5f, 1.0f, 1.0f, 0.0f},
    {0.3f, 0.2f, 0.2f, 0.15f, 0.2f},
    {1.0f, 0.1f, 0.0f, 0.1f, 0.1f},
    {0.5f, 0.3f, 0.6f, 0.5f, 0.0f},
    {0.5f, 0.5f, 0.5f, 0.5f, 0.9f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f}
};

//==============================================================================
