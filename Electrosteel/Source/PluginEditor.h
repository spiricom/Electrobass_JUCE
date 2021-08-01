/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ESModules.h"
#include "ESComponents.h"

#define EDITOR_WIDTH 900.0f
#define EDITOR_HEIGHT 700.0f

//==============================================================================
/**
*/

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class ESAudioProcessorEditor : public AudioProcessorEditor,
                               public Slider::Listener,
                               public Button::Listener,
                               public Label::Listener,
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
    void labelTextChanged(Label* label) override;
    
    void mouseDown (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    void timerCallback() override;
    
    void update();
    
    ESAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
private:
    
    void updatePedalToggle(bool state);
    void updateMPEToggle(bool state);
    void updateStringChannel(int string, int ch);
    void updateMacroControl(int macro, int ctrl);
    void updateMidiKeyRangeSlider(int min, int max);
    void updateVelocityLabel(float velocity);
    void updateRandomValueLabel(float value);
    
    TabbedComponent tabs;
    
    Component tab1;
    
    ESComponent midiKeyComponent;
    std::unique_ptr<MappingSource> midiKeySource;
    Slider midiKeyRangeSlider;
    Label midiKeyMinLabel;
    Label midiKeyMaxLabel;
    ESComponent velocityComponent;
    std::unique_ptr<MappingSource> velocitySource;
    ESComponent randomComponent;
    std::unique_ptr<MappingSource> randomSource;
    Label randomValueLabel;
    ESComponent uniqueMacroComponent;
    OwnedArray<ESDial> macroDials;
    OwnedArray<Slider> pitchBendSliders;
    MidiKeyboardComponent keyboard;
    OwnedArray<TextButton> stringActivityButtons;
    OwnedArray<OscModule> oscModules;
    OwnedArray<FilterModule> filterModules;
    std::unique_ptr<OutputModule> outputModule;
    ToggleButton pedalToggle;
    ESTabbedComponent envsAndLFOs;
    MappingSource* currentMappingSource;
    ToggleButton mpeToggle;
    Slider seriesParallelSlider;
    Label seriesLabel;
    Label parallelLabel;
    ESComponent otherSettingsComponent;
    Label transposeLabel;
    Slider transposeSlider;
    Label numVoicesLabel;
    Label numVoicesEntry;
    Label maxVoicesLabel;

    
    Component tab2;
    CopedentTable copedentTable;
    
    Component tab3;
    /* ToggleButton mpeToggle */// Declared above but will be include in this tab too
    OwnedArray<Label> macroControlEntries;
    OwnedArray<Label> macroControlLabels;
    OwnedArray<Label> stringChannelEntries;
    OwnedArray<Label> stringChannelLabels;
    
    TextButton sendOutButton;
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
