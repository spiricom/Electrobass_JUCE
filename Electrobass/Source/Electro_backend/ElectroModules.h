/*
  ==============================================================================

    ElectroModules.h
    Created: 2 Jul 2021 3:06:27pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "ElectroComponents.h"
#include "ElectroLookAndFeel.h"
#include "../sound_meter/sound_meter.h"
class ElectroAudioProcessorEditor;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

//==============================================================================

class ElectroComponent : public Component
{
public:
    ElectroComponent();
    ~ElectroComponent();
    
    void paint(Graphics &g) override;
    
    void setOutlineColour(Colour c) { outlineColour = c; }
    
protected:
    Colour outlineColour;
};

//==============================================================================

class ElectroModule : public ElectroComponent,
public Slider::Listener,
public Button::Listener,
public Label::Listener,
public ComboBox::Listener
{
public:
    
    ElectroModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&,
             float relLeftMargin, float relDialWidth, float relDialSpacing,
             float relTopMargin, float relDialHeight);
    ~ElectroModule() override;
    
    void resized() override;
    
    void sliderValueChanged(Slider* slider) override;
    void valueChangedSliderStream(Slider* slider);
    void buttonClicked(Button* button) override; 
    
    void labelTextChanged(Label* label) override {};
    void comboBoxChanged(ComboBox *comboBox) override; 
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    ElectroDial* getDial (int index);
    
protected:
    
    ElectroAudioProcessorEditor& editor;
    AudioProcessorValueTreeState& vts;
    AudioComponent& ac;
    OwnedArray<ElectroDial> dials;
    
    ToggleButton enabledToggle;
    ToggleButton smoothingToggle;
    
    float relLeftMargin, relDialWidth, relDialSpacing;
    float relTopMargin, relDialHeight;
        
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    OwnedArray<ComboBoxAttachment> comboBoxAttachments;
    
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroModule)
};

//==============================================================================

class OscModule : public ElectroModule
{
public:
    
    OscModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
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
    bool labelTextChange = false;
    
    Label harmonicsLabel;
    TextButton pitchDialToggle;
    TextButton steppedToggle;
    Label pitchLabel;
    Label freqLabel;
    ComboBox shapeCB;
    Slider sendSlider;
    Label f1Label;
    Label f2Label;
    
    TextButton syncToggle;
    TextButton syncType;
    std::unique_ptr<MappingSource> s;
    
    FileChooser* chooser;
    

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscModule)
};

//==============================================================================

class NoiseModule : public ElectroModule
{
public:
    
    NoiseModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~NoiseModule() override;
    
    void resized() override;
    void buttonClicked(Button* button) override;
    
private:
    
    Slider sendSlider;
    Label f1Label;
    Label f2Label;
    
    std::unique_ptr<MappingSource> s;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseModule)
};

//==============================================================================

class FilterModule : public ElectroModule
{
public:
    
    FilterModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~FilterModule() override;
    
    void resized() override;
    void sliderValueChanged(Slider* slider) override;

    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;
    void comboBoxChanged(ComboBox *comboBox) override
    {
        if (ac.processor.stream)
        {
            ac.processor.streamValue1 = (float)comboBox->getSelectedItemIndex()/ ((float) (filterTypeNames.size()-1));
            auto it = find(paramDestOrder.begin(), paramDestOrder.end(),comboBox->getName() );
            int index = 0;
              // If element was found
              if (it != paramDestOrder.end())
              {
                  
                  // calculating the index
                  // of K
                index = it - paramDestOrder.begin();
              }
            int tempId = index + 2;
            ac.processor.streamID1 = tempId;
            //button->get
            DBG("Send: " + comboBox->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1)/*String(streamValue)*/);
            ac.processor.streamSend = true;
        }
    };
private:
    
    ComboBox typeCB;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};

//==============================================================================

class EnvModule : public ElectroModule
{
public:
    
    EnvModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~EnvModule() override;
    
    void resized() override;
    
private:
    
    ToggleButton velocityToggle;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvModule)
};

//==============================================================================

class LFOModule : public ElectroModule
{
public:
    
    LFOModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
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

class OutputModule : public ElectroModule
{
public:
    
    OutputModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~OutputModule() override;
    void updateFXOrder(Button *button);
    void buttonClicked(Button* button) override;
    void resized() override;
    void updateFXOrder(float);
private:
    
    
    TextButton fxPostButton {"FX POST"};
    TextButton fxPreButton {"FX PRE"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputModule)
};


//==============================================================================

class FXModule : public ElectroModule
{
public:
    
    FXModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState&, AudioComponent&);
    ~FXModule() override;
    
    void resized() override;
    void paint(Graphics &g) override;
    void comboBoxChanged(ComboBox *comboBox) override;
    
private:
    ComboBox fxCB;
    void setNamesAndDefaults(FXType effect);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FXModule)
};
