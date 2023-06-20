/*
  ==============================================================================

    FXTab.h
    Created: 25 May 2022 2:45:25pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "ElectroModules.h"
#include "../Constants.h"
//==============================================================================
class FXTab : public Component
{
public:
    FXTab(ElectroAudioProcessorEditor& e, ElectroAudioProcessor& p, AudioProcessorValueTreeState& vts) :
    editor(e),
    processor(p)
    {
        for (int i = 0; i < NUM_FX; ++i)
        {
            fxModules.add(new FXModule(e, vts, *processor.fx[i]));
            addAndMakeVisible(fxModules[i]);
        }
    }
    
    ~FXTab() override
    {

    }
   
    void resized() override
    {
        for (int i = 0; i < NUM_FX; i++)
        {
            fxModules[i]->setBoundsRelative(0.01f, 0.01f +   i * 0.245f, 0.9f, 0.245f);
        }
    }
    
private:
    
    ElectroAudioProcessorEditor& editor;
    ElectroAudioProcessor& processor;
    OwnedArray<FXModule> fxModules;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FXTab)
};
