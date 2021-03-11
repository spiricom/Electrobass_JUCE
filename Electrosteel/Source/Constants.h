/*
  ==============================================================================

    Constants.h
    Created: 9 Mar 2021 11:58:01am
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#define NUM_CHANNELS 16

#define NUM_VOICES 15
#define NUM_OSC_PER_VOICE 3
#define INV_NUM_OSC_PER_VOICE 0.333333f

#define EXP_BUFFER_SIZE 128
#define DECAY_EXP_BUFFER_SIZE 512

typedef enum _ParameterGroup
{
    GroupSubtractiveKnobs = 0
} ParameterGroup;

typedef enum _SubtractiveKnobParam
{
    SubtractiveVolume = 0,
    SubtractiveShape,
    SubtractiveDetune,
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

static const std::vector<std::string> cSubtractiveKnobParamNames = {
    "SubtractiveVolume",
    "SubtractiveShape",
    "SubtractiveDetune",
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
    { 0.0f, 1.0f, 0.0f },   //SubtractiveDetune
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
