/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include "Electro_backend/FXTab.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Electro_backend/ElectroModules.h"
#include "Electro_backend/ElectroComponents.h"
#include "Constants.h"


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
                               public DragAndDropContainer,
                               public sd::SoundMeter::MetersComponent::FadersChangeListener

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
    //void currentTabChanged(int tabIndex, const String &newCurrentTabName) override;
    void update();
    
    ElectroAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    
    
private:
    
    // Updating things that don't have attachments to the vts
    void updateMPEToggle(bool state);
    void updateMuteToggle(bool state);
    void updateStringChannel(int string, int ch);
    void updateMacroControl(int macro, int ctrl);
    void updateMacroNames(int macro, String name);
    void updateMidiKeyRangeSlider(int min, int max);
    void updateNumVoicesSlider(int numVoices);
    void updateVelocityLabel(float velocity);
    void updateRandomValueLabel(float value);
    static void setVerticalRotatedWithBounds (Component& component, bool clockWiseRotation, Rectangle<int> verticalBounds)
    {
        auto angle = MathConstants<float>::pi / 2.0f;

        if (! clockWiseRotation)
            angle *= -1.0f;

        component.setTransform ({});
        component.setSize (verticalBounds.getHeight(), verticalBounds.getWidth());
        component.setCentrePosition (0, 0);
        component.setTransform (AffineTransform::rotation (angle).translated (verticalBounds.getCentreX(), verticalBounds.getCentreY()));
    }
    
    void fadersChanged (std::vector<float> faderValues) override;
    sd::SoundMeter::MetersComponent meters;
    
    TextEditor presetNameEditor;
    Slider presetNumber;
    Label presetNamelabel;
    Label presetNumberlabel;
    WaveformComponent<float> OSCILLOSCOPE;
    
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
    ToggleButton muteToggle;
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
    Component tab5;
    CopedentTable copedentTable;
    
    FXTab fxTab;
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
    juce::TooltipWindow tooltipWindow;
    
    Font euphemia;
    FileChooser chooser;
    ElectroLookAndFeel laf;
    OwnedArray<MappingSource> allSources;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroAudioProcessorEditor)
};
