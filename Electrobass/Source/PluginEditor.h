/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Electro_backend/ElectroModules.h"
#include "Electro_backend/ElectroComponents.h"

#define EDITOR_WIDTH 900.0f
#define EDITOR_HEIGHT 700.0f

//==============================================================================
/**
*/

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class ElectroAudioProcessorEditor : public AudioProcessorEditor,
                               public Slider::Listener,
                               public Button::Listener,
                               public Label::Listener,
                               public KeyListener,
                               public Timer,
                               public DragAndDropContainer
{
public:
    ElectroAudioProcessorEditor (ElectroAudioProcessor&, AudioProcessorValueTreeState& vts);
	~ElectroAudioProcessorEditor();
	
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
    
    ElectroAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
    
private:
    
    // Updating things that don't have attachments to the vts
    void updateMPEToggle(bool state);
    void updateStringChannel(int string, int ch);
    void updateMacroControl(int macro, int ctrl);
    void updateMacroNames(int macro, String name);
    void updateMidiKeyRangeSlider(int min, int max);
    void updateNumVoicesSlider(int numVoices);
    void updateVelocityLabel(float velocity);
    void updateRandomValueLabel(float value);
    
    TabbedComponent tabs;
    
    Component tab1;
    
    ElectroComponent midiKeyComponent;
    std::unique_ptr<MappingSource> midiKeySource;
    Slider midiKeyRangeSlider;
    Label midiKeyMinLabel;
    Label midiKeyMaxLabel;
    ElectroComponent velocityComponent;
    std::unique_ptr<MappingSource> velocitySource;
    ElectroComponent randomComponent;
    std::unique_ptr<MappingSource> randomSource;
    Label randomValueLabel;
    ElectroComponent uniqueMacroComponent;
    OwnedArray<ElectroDial> macroDials;
    OwnedArray<Slider> pitchBendSliders;
    MidiKeyboardComponent keyboard;
    OwnedArray<TextButton> stringActivityButtons;
    OwnedArray<OscModule> oscModules;
    std::unique_ptr<NoiseModule> noiseModule;
    OwnedArray<FilterModule> filterModules;
    std::unique_ptr<OutputModule> outputModule;
    ElectroTabbedComponent envsAndLFOs;
    MappingSource* currentMappingSource;
    ToggleButton mpeToggle;
    ElectroComponent seriesParallelComponent;
    Slider seriesParallelSlider;
    Label seriesLabel;
    Label parallelLabel;
    ElectroComponent otherSettingsComponent;
    Label numVoicesLabel;
    Slider numVoicesSlider;
    Label transposeLabel;
    Slider transposeSlider;
    
    Component tab3;
    TuningTab tuningTab;
    Component tab2;
    /* ToggleButton mpeToggle */// Declared above but will be include in this tab too
    OwnedArray<Label> macroControlEntries;
    OwnedArray<Label> macroControlNames;
    OwnedArray<Label> macroControlLabels;
    OwnedArray<Label> macroControlNameLabels;
    OwnedArray<Label> stringChannelEntries;
    OwnedArray<Label> stringChannelLabels;
    
    TextButton sendOutButton;
    Label versionLabel;
    std::unique_ptr<Drawable> logo;
    Label synderphonicsLabel;
    Label ElectrobassLabel;
    
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;

    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
   
    
    Font euphemia;
    FileChooser chooser;
    ElectroLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroAudioProcessorEditor)
};
