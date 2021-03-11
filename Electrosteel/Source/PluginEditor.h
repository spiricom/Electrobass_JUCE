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

#define EDITOR_WIDTH 800.0f
#define EDITOR_HEIGHT 600.0f

//==============================================================================
/**
*/

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class ESAudioProcessorEditor : public AudioProcessorEditor,
                               public Slider::Listener,
                               public Button::Listener,
                               public KeyListener,
                               public Timer
{
public:
    ESAudioProcessorEditor (ESAudioProcessor&, AudioProcessorValueTreeState& vts);
	~ESAudioProcessorEditor();
	
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(juce::Button* button) override;
	void buttonStateChanged(Button *button) override;
    
    bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    
    void timerCallback() override;
    
    void resizeChannelSelection();
    
    void loadWav();
    void chooseFile(const FileChooser& chooser);
    
private:
    
    ESAudioProcessor& processor;
    
    AudioProcessorValueTreeState& valueTreeState;
    
    OwnedArray<Slider> stDials;
    OwnedArray<ESButton> buttons;
    OwnedArray<ESLight> lights;
    
    OwnedArray<Slider> pitchBendSliders;
    
    MidiKeyboardComponent keyboard;
    
    OwnedArray<TextButton> channelSelection;
    
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    std::unique_ptr<Drawable> panel;
    
    OwnedArray<Label> dialLabels;
    OwnedArray<SliderAttachment> sliderAttachments;
    
    Label versionLabel;
    Font euphemia;
    juce::FileChooser chooser;
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessorEditor)
};
