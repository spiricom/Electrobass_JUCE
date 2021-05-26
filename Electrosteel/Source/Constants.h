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

#define NUM_GLOBAL_CC 16

#define NUM_VOICES 12
#define NUM_OSC_PER_VOICE 3
#define INV_NUM_OSC_PER_VOICE 0.333333f

#define NUM_CHANNELS (NUM_VOICES+1)

#define NUM_ENVS 4
#define NUM_LFOS 4

#define EXP_BUFFER_SIZE 128
#define DECAY_EXP_BUFFER_SIZE 512

//==============================================================================

typedef enum _SubtractiveKnobParam
{
    SubtractiveVolume = 0,
    SubtractiveShape,
    SubtractiveDetuneAmount,
    SubtractiveAttack,
    SubtractiveDecay,
    SubtractiveSustain,
    SubtractiveRelease,
    SubtractiveLeak,
    SubtractiveFilterAmount,
    SubtractiveFilterCutoff,
    SubtractiveFilterKeyFollow,
    SubtractiveFilterQ,
    SubtractiveFilterAttack,
    SubtractiveFilterDecay,
    SubtractiveFilterSustain,
    SubtractiveFilterRelease,
    SubtractiveFilterLeak,
    SubtractiveKnobParamNil
} SubtractiveKnobParam;

//==============================================================================

typedef enum _SawPulseParam
{
    SawPulsePitch = 0,
    SawPulseFine,
    SawPulseShape,
    SawPulseDetune,
    SawPulseVolume,
    SawPulseParamNil
} SawPulseParam;
static const StringArray cSawPulseParams = {
    "Pitch",
    "Fine",
    "Shape",
    "Detune",
    "Volume"
};
static const std::vector<std::vector<float>> vSawPulseInit = {
    { -24.0f, 24.0f, 0.0f }, //Pitch
    { -50.0f, 50.0f, 0.0f }, //Fine
    { 0.0f, 1.0f, 0.06f },  //Shape
    { 0.0f, 1.0f, 0.0f },   //Detune
    { 0.0f, 1.0f, 0.5f },   //Volume
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
    { 0.0f, 20.f, 1.0f },  //Rate
    { 0.0f, 1.0f, 0.0f },  //Shape
    { 0.0f, 1.0f, 0.0f} // Phase Offset
};

//==============================================================================

typedef enum _LowpassParam
{
    LowpassCutoff = 0,
    LowpassKeyFollow,
    LowpassResonance,
    LowpassParamNil
} LowpassParam;
static const StringArray cLowpassParams = {
    "Cutoff",
    "KeyFollow",
    "Resonance"
};
static const std::vector<std::vector<float>> vLowpassInit = {
    { 0.0f, 10000.f, 2500.f },   //Cutoff
    { 0.0f, 1.f, 0.5f },   //KeyFollow
    { 0.1f, 2.f, 0.4f },   //Resonance
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
    { 0.0f, 1.0f, 0.0f },   //Attack
    { 0.0f, 1.0f, 0.06f },  //Decay
    { 0.0f, 1.0f, 0.9f },   //Sustain
    { 0.0f, 1.0f, 0.1f },   //Release
    { 0.0f, 1.0f, 0.1f },   //Leak
};

//==============================================================================s

static const std::vector<std::string> cSubtractiveKnobParamNames = {
    "SubtractiveVolume",
    "SubtractiveShape",
    "SubtractiveDetuneAmount",
    "SubtractiveAttack",
    "SubtractiveDecay",
    "SubtractiveSustain",
    "SubtractiveRelease",
    "SubtractiveLeak",
    "SubtractiveFilterAmount",
    "SubtractiveFilterCutoff",
    "SubtractiveFilterKeyFollow",
    "SubtractiveFilterQ",
    "SubtractiveFilterAttack",
    "SubtractiveFilterDecay",
    "SubtractiveFilterSustain",
    "SubtractiveFilterRelease",
    "SubtractiveFilterLeak"
};
static const std::vector<std::vector<float>> cSubtractiveKnobParamInitValues = {
    { 0.0f, 1.0f, 0.5f },   //SubtractiveVolume
    { 0.0f, 1.0f, 0.06f },  //SubtractiveShape
    { 0.0f, 1.0f, 0.0f },   //SubtractiveDetuneAmount
    { 0.0f, 1.0f, 0.0f },   //SubtractiveAttack
    { 0.0f, 1.0f, 0.06f },  //SubtractiveDecay
    { 0.0f, 1.0f, 0.9f },   //SubtractiveSustain
    { 0.0f, 1.0f, 0.1f },   //SubtractiveRelease
    { 0.0f, 1.0f, 0.1f },   //SubtractiveLeak
    { 0.0f, 1.0f, 0.0f },   //SubtractiveFilterAmount
    { 0.0f, 1.0f, 0.5f },   //SubtractiveFilterCutoff
    { 0.0f, 1.0f, 0.2f },   //SubtractiveFilterKeyFollow
    { 0.0f, 1.0f, 0.0f },   //SubtractiveFilterQ
    { 0.0f, 1.0f, 0.0f },   //SubtractiveFilterAttack
    { 0.0f, 1.0f, 0.06f },  //SubtractiveFilterDecay
    { 0.0f, 1.0f, 0.9f },   //SubtractiveFilterSustain
    { 0.0f, 1.0f, 0.1f },   //SubtractiveFilterRelease
    { 0.0f, 1.0f, 0.1f }    //SubtractiveFilterLeak
};

//==============================================================================

typedef enum _WavetableKnobParam
{
    WavetableVolume = 0,
    WavetablePhase,
    WavetableAttack,
    WavetableDecay,
    WavetableSustain,
    WavetableRelease,
    WavetableLeak,
    WavetableFilterAmount,
    WavetableFilterCutoff,
    WavetableFilterKeyFollow,
    WavetableFilterQ,
    WavetableFilterAttack,
    WavetableFilterDecay,
    WavetableFilterSustain,
    WavetableFilterRelease,
    WavetableFilterLeak,
    WavetableKnobParamNil
} WavetableKnobParam;

static const std::vector<std::string> cWavetableKnobParamNames = {
    "WavetableVolume",
    "WavetablePhase",
    "WavetableAttack",
    "WavetableDecay",
    "WavetableSustain",
    "WavetableRelease",
    "WavetableLeak",
    "WavetableFilterAmount",
    "WavetableFilterCutoff",
    "WavetableFilterKeyFollow",
    "WavetableFilterQ",
    "WavetableFilterAttack",
    "WavetableFilterDecay",
    "WavetableFilterSustain",
    "WavetableFilterRelease",
    "WavetableFilterLeak",
};

static const std::vector<std::vector<float>> cWavetableKnobParamInitValues = {
    { 0.0f, 1.0f, 0.5f },   //WavetableVolume
    { 0.0f, 1.0f, 0.0f },   //WavetablePhase
    { 0.0f, 1.0f, 0.0f },   //WavetableAttack
    { 0.0f, 1.0f, 0.06f },  //WavetableDecay
    { 0.0f, 1.0f, 0.9f },   //WavetableSustain
    { 0.0f, 1.0f, 0.1f },   //WavetableRelease
    { 0.0f, 1.0f, 0.1f },   //WavetableLeak
    { 0.0f, 1.0f, 0.0f },   //WavetableFilterAmount
    { 0.0f, 1.0f, 0.5f },   //WavetableFilterCutoff
    { 0.0f, 1.0f, 0.2f },   //WavetableFilterKeyFollow
    { 0.0f, 1.0f, 0.0f },   //WavetableFilterQ
    { 0.0f, 1.0f, 0.0f },   //WavetableFilterAttack
    { 0.0f, 1.0f, 0.06f },  //WavetableFilterDecay
    { 0.0f, 1.0f, 0.9f },   //WavetableFilterSustain
    { 0.0f, 1.0f, 0.1f },   //WavetableFilterRelease
    { 0.0f, 1.0f, 0.1f }    //WavetableFilterLeak
};

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


