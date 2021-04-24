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

class ESAudioProcessorEditor;

//==============================================================================

class MappingSource : public Component
{
public:
    MappingSource(ESAudioProcessorEditor& editor, const String &name, float* sources, int n, Colour colour);
    ~MappingSource() override;
    
    Colour getColour() { return colour; }
    
    void resized() override;
    
    float* getValuePointer();
    int getNumSourcePointers();
    
    Label label;
    DrawableButton button;
    
private:

    std::unique_ptr<Drawable> image;
    float* source;
    int numSourcePointers;
    
    Colour colour;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingSource)
};

//==============================================================================

class MappingTarget : public Slider,
                      public DragAndDropTarget
{
public:
    
    MappingTarget(const String &name, OwnedArray<SmoothedParameter>& target, int index);
    ~MappingTarget() override;
    
    String getTextFromValue(double value) override { return text; }
    Colour getColour() { return colour; }
    
    bool isInterestedInDragSource(const SourceDetails &dragSourceDetails) override;
    void itemDropped(const SourceDetails &dragSourceDetails) override;
    
    void resized() override;
    
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;

    void setText(String s);
    void setTextColour(Colour colour);
    
    void setMapping(MappingSource* source, float start, float end, HookOperation op);
    void removeMapping();
    
    void setMappingRange(float start, float end);
    
    static void menuCallback(int result, MappingTarget* target);
    
private:
    
    String text;
    OwnedArray<SmoothedParameter>& target;
    int index;
    bool sliderEnabled;
    
    Colour colour;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingTarget)
};

//==============================================================================

class ESDial : public Component,
               public Slider::Listener
{
public:
    
    ESDial(ESAudioProcessorEditor& editor, const String& name);
    ESDial(ESAudioProcessorEditor& editor, const String& name, Colour colour, float* source, int n);
    ESDial(ESAudioProcessorEditor& editor, const String& name, OwnedArray<SmoothedParameter>& target);
    ~ESDial() override;
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void sliderValueChanged(Slider* slider) override;
    
    void setText (const String& newText, NotificationType notification);
    void setFont (const Font& newFont);
    
    MappingTarget* getTarget(int index);
    MappingSource* getSource();
    
    Slider& getSlider() { return slider; }
    
private:
    
    Slider slider;
    OwnedArray<MappingTarget> t;
    std::unique_ptr<MappingSource> s;
    Label label;
    
    ESLookAndFeel laf;
    
    static const int numTargets = 3;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESDial)
};

//==============================================================================

class ConnectionsContainer : public Component,
                             public Timer
{
public:
    ConnectionsContainer()
    {
        cursor = Drawable::createFromImageData(BinaryData::mappingtargeticon_svg,
                                               BinaryData::mappingtargeticon_svgSize);
        addChildComponent(cursor.get());
        
        setInterceptsMouseClicks(false, false);
        
        startTimer(10);
    }
    ~ConnectionsContainer() override {};
    
    void paint(Graphics &g) override
    {
        Point<float> mousePos = getMouseXYRelative().toFloat();
        cursor->drawWithin(g, Rectangle<float>(10.f, 10.f).withCentre(mousePos),
                           RectanglePlacement::fillDestination, 1.0f);
        if (incompleteConnection) connections.getLast()->setEnd(mousePos);
        g.setColour(Colours::gold);
        for (auto line : connections) g.drawLine(*line);
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    void startConnection(float x, float y)
    {
        connections.add(new Line<float>(x, y, x, y));
        incompleteConnection = true;
    }

    std::unique_ptr<Drawable> cursor;
    OwnedArray<Line<float>> connections;
    
private:

    bool incompleteConnection;
};

class ESTabbedComponent : public TabbedComponent
{
public:
    using TabbedComponent::TabbedComponent;
    
    void currentTabChanged (int newCurrentTabIndex, const String &newCurrentTabName) override
    {
        TabbedButtonBar& bar = getTabbedButtonBar();
        for (int i = 0; i < bar.getNumTabs(); ++i)
        {
            bar.getTabButton(i)->setAlpha(i == getCurrentTabIndex() ? 1.0f : 0.7f);
        }
    }
};
