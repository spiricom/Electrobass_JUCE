/*
  ==============================================================================

    ElectroUI.h
    Created: 13 Jul 2022 1:55:06pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "Electro_backend/FXTab.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Electro_backend/ElectroModules.h"
#include "Electro_backend/ElectroComponents.h"
#include "Constants.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class ElectroUI : public TabbedComponent,
                               public Slider::Listener,
                               public Button::Listener,
                               public Label::Listener,
                               public Timer,
                               public DragAndDropContainer
                               
{
public:
    ElectroUI (ElectroAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~ElectroUI() override;
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void labelTextChanged(Label* label) override;
    
    void mouseDown (const MouseEvent &event) override;
    //bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    void timerCallback() override;
    //void currentTabChanged(int tabIndex, const String &newCurrentTabName) override;
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
    ElectroLookAndFeel laf;
    WaveformComponent<float> OSCILLOSCOPE;
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
    
    Label numVoicesLabel;
    Slider numVoicesSlider;
    Label transposeLabel;
    Slider transposeSlider;
    
    Slider rangeSlider;
    
    
    
    
    Label rangeLabel;
    Component tab3;
    TuningTab tuningTab;
    Component tab2;
    Component tab4;
    FXTab fxTab;
    
    /* ToggleButton mpeToggle */// Declared above but will be include in this tab too
    OwnedArray<Label> macroControlEntries;
    OwnedArray<Label> macroControlNames;
    OwnedArray<Label> macroControlLabels;
    OwnedArray<Label> macroControlNameLabels;
    OwnedArray<Label> stringChannelEntries;
    OwnedArray<Label> stringChannelLabels;
    
    
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;

    
    
    
    FileChooser chooser;
    
    OwnedArray<MappingSource> allSources;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroUI);
};
