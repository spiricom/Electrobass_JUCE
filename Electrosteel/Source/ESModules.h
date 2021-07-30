/*
  ==============================================================================

    ESModules.h
    Created: 2 Jul 2021 3:06:27pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ESComponents.h"
#include "ESLookAndFeel.h"

class ESAudioProcessorEditor;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

//==============================================================================

class ESComponent : public Component
{
public:
    ESComponent();
    ~ESComponent();
    
    void paint(Graphics &g) override;
    
    void setOutlineColour(Colour c) { outlineColour = c; }
    
protected:
    Colour outlineColour;
};

//==============================================================================

class ESModule : public ESComponent,
public Slider::Listener,
public Button::Listener,
public Label::Listener,
public ComboBox::Listener
{
public:
    
    ESModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&,
             float relLeftMargin, float relDialWidth, float relDialSpacing,
             float relTopMargin, float relDialHeight);
    ~ESModule() override;
    
    void resized() override;
    
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button* button) override {};
    void labelTextChanged(Label* label) override {};
    void comboBoxChanged(ComboBox *comboBox) override {};
    
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
    void buttonClicked(Button* button) override;
    void labelTextChanged(Label* label) override;
    void comboBoxChanged(ComboBox *comboBox) override;
    
    void mouseDown(const MouseEvent &) override;
    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    
    void updateShapeCB();
    
    void displayPitch();
    void displayPitchMapping(MappingTarget* mt);
    
private:
    
    Label pitchLabel;
    Label freqLabel;
    ComboBox shapeCB;
    Slider sendSlider;
    Label f1Label;
    Label f2Label;
    
    std::unique_ptr<MappingSource> s;
    
    FileChooser chooser;
    
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
    void comboBoxChanged(ComboBox *comboBox) override;
    
    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    
    void displayRate();
    void displayRateMapping(MappingTarget* mt);
    
private:
    
    Label rateLabel;
    ComboBox shapeCB;
    ToggleButton syncToggle;
    
    FileChooser chooser;
    
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
