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
class OscModule;
class LFOModule;
class FilterModule;
class OutputModule;

//==============================================================================
/**
*/

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class ESAudioProcessorEditor : public AudioProcessorEditor,
                               public Slider::Listener,
                               public Button::Listener,
                               public KeyListener,
                               public Timer,
                               public DragAndDropContainer
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
    
    void mouseDown (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    void timerCallback() override;
    
    void loadWav();
    void chooseFile(const FileChooser& chooser);
    
    void getAllChildren(Component* component, Array<Component*> &children);
    Array<Component*> getAllChildren();
    
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
private:
    
    TabbedComponent tabs;
    
    Component tab1;
    OwnedArray<ESDial> macroDials;
    OwnedArray<Slider> pitchBendSliders;
    MidiKeyboardComponent keyboard;
    OwnedArray<TextButton> channelStringButtons;
    OwnedArray<OscModule> oscModules;
    OwnedArray<FilterModule> filterModules;
    std::unique_ptr<OutputModule> outputModule;
    ESTabbedComponent envsAndLFOs;
    MappingSource* currentMappingSource;
    OwnedArray<TextButton> copedentButtons;
    ToggleButton mpeToggle;
    Slider seriesParallelSlider;
    Label seriesLabel;
    Label parallelLabel;
    
    Component tab2;
    CopedentTable copedentTable;
    
    Label versionLabel;
    std::unique_ptr<Drawable> logo;
    Label synderphonicsLabel;
    Label electrosteelLabel;
    
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;

    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    
    Font euphemia;
    FileChooser chooser;
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESAudioProcessorEditor)
};

//==============================================================================

class ESModule : public Component,
                 public Slider::Listener,
                 public Label::Listener
{
public:
    
    ESModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&,
             float relLeftMargin, float relDialWidth, float relDialSpacing,
             float relTopMargin, float relDialHeight);
    ~ESModule() override;
    
    void resized() override;
    void paint(Graphics &g) override;
    
    void sliderValueChanged(Slider* slider) override {};
    void labelTextChanged(Label* label) override {};
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    ESDial* getDial (int index);
    
protected:
    
    ESAudioProcessorEditor& editor;
    AudioProcessorValueTreeState& vts;
    AudioComponent& ac;
    OwnedArray<ESDial> dials;
    
    ToggleButton enabledToggle;
    
    float relLeftMargin, relDialWidth, relDialSpacing;
    float relTopMargin, relDialHeight;
    
    Colour outlineColour;
    
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    OwnedArray<ComboBoxAttachment> comboBoxAttachments;
    
    ESLookAndFeel laf;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESModule)
};

//==============================================================================

class OscModule : public ESModule
{
public:
    
    OscModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~OscModule() override;
    
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void labelTextChanged(Label* label) override;
    
    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    
    void displayPitch();
    void displayPitchMapping(MappingTarget* mt);
    
private:
    
    Label pitchLabel;
    ComboBox shapeCB;
    Slider sendSlider;
    Label f1Label;
    Label f2Label;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscModule)
};

//==============================================================================

class FilterModule : public ESModule
{
public:
    
    FilterModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~FilterModule() override;
    
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void labelTextChanged(Label* label) override;
    
    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    
    void displayCutoff();
    void displayCutoffMapping(MappingTarget* mt);
    
private:
    
    Label cutoffLabel;
    ComboBox typeCB;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};

//==============================================================================

class EnvModule : public ESModule
{
public:
    
    EnvModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~EnvModule() override;
    
    void resized() override;
    
private:
    
    ToggleButton velocityToggle;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvModule)
};

//==============================================================================

class LFOModule : public ESModule
{
public:
    
    LFOModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~LFOModule() override;
    
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void labelTextChanged(Label* label) override;
    
    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    
    void displayRate();
    void displayRateMapping(MappingTarget* mt);
    
private:
    
    Label rateLabel;
    ComboBox shapeCB;
    ToggleButton syncToggle;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOModule)
};

//==============================================================================

class OutputModule : public ESModule
{
public:
    
    OutputModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~OutputModule() override;
    
    void resized() override;
    
private:
    
    std::unique_ptr<ESDial> masterDial;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputModule)
};
