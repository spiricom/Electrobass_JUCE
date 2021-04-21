/*
 ==============================================================================
 
 ESComponents.h
 Created: 19 Feb 2021 12:42:05pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ESLookAndFeel.h"

//==============================================================================

class AdaptiveSlider : public Slider
{
public:
    using Slider::Slider;
    
    double getValueFromText (const String& text) override;
};

//==============================================================================

class ESDial : public Component
{
public:
    
    ESDial(const String& t);
    ~ESDial() override;
    
    void addListener (Slider::Listener* l) { slider.addListener(l); }
    void removeListener (Slider::Listener* l) { slider.removeListener(l); }
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    void setSliderBounds (float x, float y, float w, float h);
    void setSliderBounds (Rectangle<float> newBounds);

    void setLabelBounds (float x, float y, float w, float h);
    void setLabelBounds (Rectangle<float> newBounds);
    
    void setText (const String& newText, NotificationType notification);
    void setFont (const Font& newFont);
    
    Slider& getSlider() { return slider; }
    
private:

    Slider slider;
    Label label;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESDial)
};


//==============================================================================
// Copy of juce::ShapeButton in case we want to change/add things
class ESButton : public Button
{
public:
    
    ESButton (const String& name,
                   Colour normalColour,
                   Colour overColour,
                   Colour downColour);
    
    /** Destructor. */
    ~ESButton() override;
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    void setShape (const Path& newShape,
                   bool resizeNowToFitThisShape,
                   bool maintainShapeProportions,
                   bool hasDropShadow);
    
    void setColours (Colour normalColour,
                     Colour overColour,
                     Colour downColour);
    
    
    void setOnColours (Colour normalColourOn,
                       Colour overColourOn,
                       Colour downColourOn);
    
    void shouldUseOnColours (bool shouldUse);
    
    void setOutline (Colour outlineColour, float outlineStrokeWidth);
    
    void setBorderSize (BorderSize<int> border);
    
    void paintButton (Graphics&, bool, bool) override;
    
private:
    
    Colour normalColour,   overColour,   downColour,
    normalColourOn, overColourOn, downColourOn, outlineColour;
    bool useOnColours;
    DropShadowEffect shadow;
    Path shape;
    BorderSize<int> border;
    bool maintainShapeProportions;
    float outlineWidth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESButton)
};

//==============================================================================
class ESLight : public Component,
public SettableTooltipClient
{
public:
    
    ESLight(const String& name,
                 Colour normalColour,
                 Colour onColour);
    ~ESLight() override;
    
    void setBounds (float x, float y, float d);
    void setBounds (Rectangle<float> newBounds);
    
    void setState (bool state);
    void setBrightness (float newBrightness);
    
    void paint (Graphics &g) override;
    
private:
    
    Colour normalColour, onColour;
    bool isOn;
    float brightness;
    float lightSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESLight)
};

//==============================================================================

class MappingMenu : public Component,
                    public Button::Listener
{
public:
    
    MappingMenu();
    ~MappingMenu() override;
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    void buttonClicked(Button* button) override;
    
    TabbedComponent menu;
    
private:
    
    std::unique_ptr<Drawable> image;
    DrawableButton closeButton;
    ArrowButton moveLeftButton;
    ArrowButton moveRightButton;
    DrawableRectangle outline;
    DrawableRectangle background;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingMenu)
};

//==============================================================================

class MappingSource : public DrawableButton
{
public:
    
    MappingSource(const String &name, float* source);
    ~MappingSource() override;
    
    float* getValuePointer();
    
private:
    
    std::unique_ptr<Drawable> image;
    float* source;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingSource)
};

//==============================================================================

class MappingTarget : public DrawableButton
{
public:
    
    MappingTarget(const String &name, SmoothedParameter& target);
    ~MappingTarget() override;
    
    void setBounds (float x, float y, float w, float h);
    void setBounds (Rectangle<float> newBounds);
    
    void viewMappings(int index = -1);
    void createMapping(MappingSource* source, bool updateCurrentTab);
    
    SmoothedParameter& getParameter();
    
private:
    
    std::unique_ptr<Drawable> image;
    SmoothedParameter& target;
    
    MappingMenu mappings;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingTarget)
};

//==============================================================================

class MappingEditor : public Component,
                      public Slider::Listener,
                      public Button::Listener,
                      public ComboBox::Listener
{
public:
    
    MappingEditor(MappingSource &source, MappingTarget &target, ParameterHook &hook);
    ~MappingEditor() override;
    
    void resized() override;
    
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button* button) override;
    void comboBoxChanged (ComboBox *comboBox) override;
    
    MappingSource& getSource();
    MappingTarget& getTarget();
    
private:
    
    MappingSource& source;
    MappingTarget& target;
    ParameterHook& hook;
    
    ComboBox operationSelect;
    AdaptiveSlider startSlider;
    Label startLabel;
    AdaptiveSlider endSlider;
    Label endLabel;
    DrawableButton deleteButton;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingEditor)
};
