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

#define EDITOR_WIDTH 900.0f
#define EDITOR_HEIGHT 700.0f

class ESModule;

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
    
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
private:
    
    std::unique_ptr<ESDial> masterDial;
    OwnedArray<ESDial> ccDials;
    OwnedArray<Slider> pitchBendSliders;
    
    MidiKeyboardComponent keyboard;
    OwnedArray<TextButton> channelSelection;
    
    OwnedArray<ESModule> modules;

    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    std::unique_ptr<Drawable> panel;
    
    OwnedArray<SliderAttachment> sliderAttachments;
    
    Label versionLabel;
    Font euphemia;
    juce::FileChooser chooser;
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessorEditor)
};

//==============================================================================

class ESModule : public Component
{
public:
    
    ESModule(AudioProcessorValueTreeState&, AudioComponent&);
    ~ESModule() override;
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
private:
    
    AudioProcessorValueTreeState& vts;
    AudioComponent& ac;
    OwnedArray<ESDial> dials;
    
    OwnedArray<SliderAttachment> sliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESModule)
};
