/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ESComponents.h"

//==============================================================================
/**
*/

#define EDITOR_WIDTH 800.0f
#define EDITOR_HEIGHT 600.0f

class ESAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener, public Button::Listener, public Timer
{
public:
    ESAudioProcessorEditor (ESAudioProcessor&);
	~ESAudioProcessorEditor();
	
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(juce::Button* button) override;
	void buttonStateChanged(Button *button) override;
    
    void timerCallback() override;
    
    void loadWav();
    void chooseFile(const FileChooser& chooser);

    OwnedArray<Slider> dials;
    OwnedArray<ESButton> buttons;
    OwnedArray<ESLight> lights;
    
private:
    
    ESAudioProcessor& processor;
    
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    std::unique_ptr<Drawable> panel;
    
    OwnedArray<Label> dialLabels;
    
    Label versionLabel;
    
    Font euphemia;
    
    juce::FileChooser chooser;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessorEditor)
};
