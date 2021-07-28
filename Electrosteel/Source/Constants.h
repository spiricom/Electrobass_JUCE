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

#define NUM_MACROS 16

#define NUM_STRINGS 12

#define NUM_CHANNELS (NUM_STRINGS+1)

#define NUM_OSCS 3
#define INV_NUM_OSCS 0.333333f
#define NUM_FILT 2
#define NUM_ENVS 4
#define NUM_LFOS 4

#define EXP_BUFFER_SIZE 2048
#define DECAY_EXP_BUFFER_SIZE 2048

#define MAX_NUM_UNIQUE_SKEWS 10

//==============================================================================

typedef enum _OscParam
{
    OscPitch = 0,
    OscFine,
    OscShape,
    OscAmp,
    OscParamNil
} OscParam;
static const StringArray cOscParams = {
    "Pitch",
    "Fine",
    "Shape",
    "Amp"
};
static const std::vector<std::vector<float>> vOscInit = {
    { -24.0f, 24.0f, 0.0f, 0.0f }, //Pitch
    { -100.f, 100.f, 0.0f, 0.0f }, //Fine
    { 0.0f, 1.0f, 0.0f, 0.5f },  //Shape
    { 0.0f, 2.0f, 1.0f, 1.0f },  //Amp
};

typedef enum _OscShapeSet
{
    SawPulseOscShapeSet = 0,
    SineTriOscShapeSet,
    UserOscShapeSet,
    OscShapeSetNil
} OscShapeSet;
static const StringArray oscShapeSetNames = {
    "Saw-Pulse",
    "Sine-Tri",
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
    { 0.0f, 30.f, 1.0f, 2.f },  //Rate
    { 0.0f, 1.0f, 0.0f, 0.5f },  //Shape
    { 0.0f, 1.0f, 0.0f, 0.5f } // Phase Offset
};

typedef enum _LFOShapeSet
{
    SineTriLFOShapeSet = 0,
    SawPulseLFOShapeSet,
    LFOShapeSetNil
} LFOShapeSet;
static const StringArray lfoShapeSetNames = {
    "Sine-Tri",
    "Saw-Square"
};

//==============================================================================

typedef enum _FilterParam
{
    FilterCutoff = 0,
    FilterResonance,
    FilterKeyFollow,
    FilterParamNil
} FilterParam;
static const StringArray cFilterParams = {
    "Cutoff",
    "Resonance",
    "KeyFollow"
};
static const std::vector<std::vector<float>> vFilterInit = {
    { 0.0f, 127.f, 72.f, 63.5f },   //Cutoff
    { 0.1f, 10.0f, 0.5f, 0.7f },   //Resonance
    { 0.0f, 1.f, 0.5f, 0.5f }   //KeyFollow
};

typedef enum _FilterType
{
    LowpassFilter = 0,
    HighpassFilter,
    BandpassFilter,
    FilterTypeNil
} FilterType;
static const StringArray filterTypeNames = {
    "Lowpass",
    "Highpass",
    "Bandpass"
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
    { 0.0f, 1.f, 0.8f, 0.5f },   //Sustain
    { 0.0f, 20000.0f, 20.f, 4000.f },   //Release
    { 0.0f, 1.0f, 0.f, 0.5f },   //Leak
};

//==============================================================================

typedef enum _OutputParam
{
    OutputAmp,
    OutputPan,
    OutputParamNil
} OutputParam;
static const StringArray cOutputParams = {
    "Amp",
    "Pan"
};
static const std::vector<std::vector<float>> vOutputInit = {
    { 0.0f, 2.0f, 0.0f, 1.0f },   //Amp
    { -1.0f, 1.0f, 0.f, 0.f},  //Pan
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
