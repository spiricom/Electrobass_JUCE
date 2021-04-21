/*
  ==============================================================================

    ESComponents.cpp
    Created: 19 Feb 2021 12:42:05pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "ESComponents.h"

double AdaptiveSlider::getValueFromText (const String& text)
{
    auto t = text.trimStart();
    
    if (t.endsWith (getTextValueSuffix()))
        t = t.substring (0, t.length() - getTextValueSuffix().length());
    
    if (valueFromTextFunction != nullptr)
        return valueFromTextFunction (t);
    
    while (t.startsWithChar ('+'))
        t = t.substring (1).trimStart();
    
    double value = t.initialSectionContainingOnly ("0123456789.,-")
    .getDoubleValue();
    
    if (value < getMinimum()) setRange(value, getMaximum());
    else if (value > getMaximum()) setRange(getMinimum(), value);
    return value;
}

//==============================================================================
//==============================================================================

ESDial::ESDial(const String& t) :
label(t, t)
{
    setInterceptsMouseClicks(false, true);
    
    slider.setLookAndFeel(&laf);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 4, 4);
    slider.setRange(0., 1.);
    addAndMakeVisible(&slider);
    
    label.setJustificationType(Justification::centredTop);
    label.setLookAndFeel(&laf);
    addAndMakeVisible(&label);
}

ESDial::~ESDial()
{
    slider.setLookAndFeel(nullptr);
    label.setLookAndFeel(nullptr);
}

void ESDial::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void ESDial::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
}

void ESDial::setSliderBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setSliderBounds(newBounds);
}

void ESDial::setSliderBounds (Rectangle<float> newBounds)
{
    slider.setBounds(newBounds.toNearestInt());
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, newBounds.getWidth(),
                           newBounds.getHeight() - newBounds.getWidth());
}

void ESDial::setLabelBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setLabelBounds(newBounds);
}

void ESDial::setLabelBounds (Rectangle<float> newBounds)
{
    label.setBounds(newBounds.toNearestInt());
}

void ESDial::setText (const String& newText, NotificationType notification)
{
    label.setText(newText, notification);
}

void ESDial::setFont (const Font& newFont)
{
    label.setFont(newFont);
}

//==============================================================================
//==============================================================================

ESButton::ESButton (const String& t, Colour n, Colour o, Colour d)
: Button (t),
normalColour   (n), overColour   (o), downColour   (d),
normalColourOn (n), overColourOn (o), downColourOn (d),
useOnColours(false),
maintainShapeProportions (false),
outlineWidth (0.0f)
{
}

ESButton::~ESButton() {}

void ESButton::setColours (Colour newNormalColour, Colour newOverColour, Colour newDownColour)
{
    normalColour = newNormalColour;
    overColour   = newOverColour;
    downColour   = newDownColour;
}

void ESButton::setOnColours (Colour newNormalColourOn, Colour newOverColourOn, Colour newDownColourOn)
{
    normalColourOn = newNormalColourOn;
    overColourOn   = newOverColourOn;
    downColourOn   = newDownColourOn;
}

void ESButton::shouldUseOnColours (bool shouldUse)
{
    useOnColours = shouldUse;
}

void ESButton::setOutline (Colour newOutlineColour, const float newOutlineWidth)
{
    outlineColour = newOutlineColour;
    outlineWidth = newOutlineWidth;
}

void ESButton::setBorderSize (BorderSize<int> newBorder)
{
    border = newBorder;
}

void ESButton::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> bounds (x, y, w, h);
    Component::setBounds(bounds.toNearestInt());
}

void ESButton::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
}

void ESButton::setShape (const Path& newShape,
                            const bool resizeNowToFitThisShape,
                            const bool maintainShapeProportions_,
                            const bool hasShadow)
{
    shape = newShape;
    maintainShapeProportions = maintainShapeProportions_;
    
    shadow.setShadowProperties (DropShadow (Colours::black.withAlpha (0.5f), 3, Point<int>()));
    setComponentEffect (hasShadow ? &shadow : nullptr);
    
    if (resizeNowToFitThisShape)
    {
        auto newBounds = shape.getBounds();
        
        if (hasShadow)
            newBounds = newBounds.expanded (4.0f);
        
        shape.applyTransform (AffineTransform::translation (-newBounds.getX(),
                                                            -newBounds.getY()));
        
        setSize (1 + (int) (newBounds.getWidth()  + outlineWidth) + border.getLeftAndRight(),
                 1 + (int) (newBounds.getHeight() + outlineWidth) + border.getTopAndBottom());
    }
    
    repaint();
}

void ESButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(-getX(),-getY()), Colour(10, 10, 10), juce::Point<float>(-getX(), getParentHeight()-getY()), false));
    g.fillRect(getLocalBounds());
    
    if (! isEnabled())
    {
        shouldDrawButtonAsHighlighted = false;
        shouldDrawButtonAsDown = false;
    }
    
    auto r = border.subtractedFrom (getLocalBounds())
    .toFloat()
    .reduced (outlineWidth * 0.5f);

    if (shouldDrawButtonAsDown)
    {
        const float sizeReductionWhenPressed = 0.04f;
        
        r = r.reduced (sizeReductionWhenPressed * r.getWidth(),
                       sizeReductionWhenPressed * r.getHeight());
    }
    
    auto trans = shape.getTransformToScaleToFit (r, maintainShapeProportions);
    
    if      (shouldDrawButtonAsDown)        g.setColour (getToggleState() && useOnColours ? downColourOn   : downColour);
    else if (shouldDrawButtonAsHighlighted) g.setColour (getToggleState() && useOnColours ? overColourOn   : overColour);
    else                                    g.setColour (getToggleState() && useOnColours ? normalColourOn : normalColour);
    
    g.fillPath (shape, trans);
    
    if (outlineWidth > 0.0f)
    {
        g.setColour (outlineColour);
        g.strokePath (shape, PathStrokeType (outlineWidth), trans);
    }
}

//==============================================================================
//==============================================================================

ESLight::ESLight(const String& name, Colour normalColour, Colour onColour) :
Component(name),
normalColour(normalColour),
onColour(onColour),
isOn(false),
brightness(1.0f),
lightSize(5.0f)
{
    setPaintingIsUnclipped(true);
}

ESLight::~ESLight()
{
}

void ESLight::setBounds (float x, float y, float d)
{
    Rectangle<float> newBounds (x, y, d, d);
    setBounds(newBounds);
}

void ESLight::setBounds (Rectangle<float> newBounds)
{
    lightSize = newBounds.getWidth() * 0.25f;
    Component::setBounds(newBounds.expanded(lightSize, lightSize).toNearestInt());
}

void ESLight::setState (bool state)
{
    if (state == isOn) return;
    isOn = state;
    repaint();
}

void ESLight::setBrightness (float newBrightness)
{
    if (newBrightness == brightness) return;
    brightness = newBrightness;

    repaint();
}

void ESLight::paint (Graphics &g)
{
    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(-getX(),-getY()), Colour(10, 10, 10), juce::Point<float>(-getX(), getParentHeight()-getY()), false));
    g.fillRect(getLocalBounds());
    
    Rectangle<float> area = getLocalBounds().toFloat();
    Rectangle<float> innerArea = area.reduced(lightSize, lightSize);
    g.setColour(normalColour.interpolatedWith(onColour, isOn ? (brightness * 0.5f) : 0.0f));
    g.fillEllipse(innerArea);
    
    if (isOn)
    {
        float r = area.getWidth() * 0.5f * (1.0f - brightness);
        g.setGradientFill(ColourGradient(onColour, innerArea.getCentre(), onColour.withAlpha(0.0f),
                                         juce::Point<float>(area.getCentreX(), area.getY() + r),
                                         true));
        g.fillEllipse(area);
    }
}

//==============================================================================
//==============================================================================

MappingMenu::MappingMenu() :
menu(TabbedButtonBar::Orientation::TabsAtTop),
closeButton("Close", DrawableButton::ButtonStyle::ImageFitted),
moveLeftButton("Move Left", 0.5f, Colours::gold),
moveRightButton("Move Right", 0.0f, Colours::gold)
{
    menu.setColour(TabbedComponent::outlineColourId, Colours::darkgrey);
    addAndMakeVisible(&menu);
    
    image = Drawable::createFromImageData(BinaryData::closeicon_svg, BinaryData::closeicon_svgSize);
    closeButton.setImages(image.get());
    closeButton.setEdgeIndent(0);
    closeButton.setAlwaysOnTop(true);
    closeButton.addListener(this);
    addAndMakeVisible(&closeButton);

    moveLeftButton.setAlwaysOnTop(true);
    moveLeftButton.addListener(this);
    addAndMakeVisible(&moveLeftButton);
    
    moveRightButton.setAlwaysOnTop(true);
    moveRightButton.addListener(this);
    addAndMakeVisible(&moveRightButton);
    
    outline.setFill(Colours::darkgrey);
    addAndMakeVisible(&outline);
    addAndMakeVisible(&background);
}

MappingMenu::~MappingMenu()
{
    
}

void MappingMenu::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void MappingMenu::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
    Rectangle<int> mainArea = getLocalBounds();
    menu.setBounds(mainArea);
    
    float w = mainArea.getWidth();
    float m = w * 0.01f;
    float x = mainArea.getX();
    float y = mainArea.getY() + m + menu.getTabBarDepth();
    float s = w * 0.035f;
    closeButton.setBounds(x + w - (m + s), y, s, s);
    
    moveLeftButton.setBounds(x + m, y, s, s);
    moveRightButton.setBounds(x + m + s * 1.5f, y, s, s);
    
    background.setBounds(getLocalBounds().reduced(1));
    background.setRectangle(getLocalBounds().reduced(1).toFloat());
    background.toBack();
    outline.setBounds(getLocalBounds());
    outline.setRectangle(getLocalBounds().toFloat());
    outline.toBack();
}

void MappingMenu::buttonClicked(Button* button)
{
    if (DrawableButton* db = dynamic_cast<DrawableButton*>(button))
    {
        getParentComponent()->removeChildComponent(this);
    }
    if (ArrowButton* ab = dynamic_cast<ArrowButton*>(button))
    {
        int i = menu.getCurrentTabIndex();
        int n = i;
        MappingEditor* mapping = dynamic_cast<MappingEditor*>(menu.getTabContentComponent(i));
        if (ab == &moveLeftButton)
        {
            n = (i-1) % menu.getNumTabs();
        }
        else if (ab == &moveRightButton)
        {
            n = (i+1) % menu.getNumTabs();
        }
        mapping->getTarget().getParameter().moveHook(i, n);
        menu.moveTab(i, n);
    }
}

//==============================================================================
//==============================================================================

MappingSource::MappingSource(const String &name, float* source) :
DrawableButton(name, ButtonStyle::ImageFitted),
source(source)
{
    image = Drawable::createFromImageData(BinaryData::mappingsourceicon_svg,
                                          BinaryData::mappingsourceicon_svgSize);
    setImages(image.get());
}

MappingSource::~MappingSource()
{
    source = nullptr;
}

float* MappingSource::getValuePointer()
{
    return source;
}

//==============================================================================
//==============================================================================

MappingTarget::MappingTarget(const String &name, SmoothedParameter& target) :
DrawableButton(name, ButtonStyle::ImageFitted),
target(target)
{
    image = Drawable::createFromImageData(BinaryData::mappingtargeticon_svg,
                                          BinaryData::mappingtargeticon_svgSize);
    setImages(image.get());
}

MappingTarget::~MappingTarget()
{
}

void MappingTarget::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void MappingTarget::setBounds (Rectangle<float> newBounds)
{
    DrawableButton::setBounds(newBounds.toNearestInt());
    Component* parent = getParentComponent();
    mappings.setBounds(parent->getLocalBounds()
                       .reduced(parent->getWidth() * 0.2f, parent->getHeight() * 0.2f)
                       .translated(0, -parent->getHeight() * 0.1f)
                       .toFloat());
}

void MappingTarget::viewMappings(int index)
{
    if (mappings.menu.getNumTabs() > 0)
    {
        if (index >= 0) mappings.menu.setCurrentTabIndex(index);
        getParentComponent()->addAndMakeVisible(&mappings);
    }
}

void MappingTarget::createMapping(MappingSource* source, bool updateCurrentTab)
{
    if (source == nullptr) return;
    
    ParameterHook& hook = target.addHook(source->getValuePointer(), 0.0f, 1.0f, HookAdd);
    mappings.menu.addTab(source->getName(), Colours::black, new MappingEditor(*source, *this, hook), true);
    if (updateCurrentTab) mappings.menu.setCurrentTabIndex(mappings.menu.getNumTabs() - 1);
}

SmoothedParameter& MappingTarget::getParameter()
{
    return target;
}

//==============================================================================
//==============================================================================

MappingEditor::MappingEditor(MappingSource &source, MappingTarget &target, ParameterHook &hook) :
source(source),
target(target),
hook(hook),
deleteButton("Delete Mapping", DrawableButton::ImageFitted)
{
    float r = (target.getParameter().getEnd() - target.getParameter().getStart()) * 0.5f;
    
    operationSelect.setJustificationType(Justification::centred);
    operationSelect.addItem("Add", HookAdd+1);
    operationSelect.addItem("Multiply", HookMultiply+1);
    operationSelect.setSelectedItemIndex(0);
    operationSelect.addListener(this);
    addAndMakeVisible(&operationSelect);
    
    startSlider.setName("Start");
    startSlider.setSliderStyle(Slider::LinearBar);
    startSlider.setRange(-r, r);
    startSlider.setValue(hook.min);
    startSlider.setDoubleClickReturnValue(true, 0.0f);
    startSlider.addListener(this);
    addAndMakeVisible(&startSlider);
    
    endSlider.setName("End");
    endSlider.setSliderStyle(Slider::LinearBar);
    endSlider.setRange(-r, r);
    endSlider.setValue(hook.max);
    endSlider.setDoubleClickReturnValue(true, 0.0f);
    endSlider.addListener(this);
    addAndMakeVisible(&endSlider);
    
    startLabel.setText("min", dontSendNotification);
    startLabel.setBorderSize(BorderSize<int>(0));
    startLabel.setJustificationType(Justification::centredLeft);
    startLabel.setLookAndFeel(&laf);
    addAndMakeVisible(&startLabel);
    
    endLabel.setText("max", dontSendNotification);
    endLabel.setBorderSize(BorderSize<int>(0));
    endLabel.setJustificationType(Justification::centredLeft);
    endLabel.setLookAndFeel(&laf);
    addAndMakeVisible(&endLabel);
    
//    image = Drawable::createFromImageData(BinaryData::closeicon_svg, BinaryData::closeicon_svgSize);
//    deleteButton.setImages(image.get());
//    deleteButton.setEdgeIndent(0);
//    deleteButton.setAlwaysOnTop(true);
//    deleteButton.addListener(this);
//    addAndMakeVisible(&deleteButton);
}

MappingEditor::~MappingEditor()
{
    startLabel.setLookAndFeel(nullptr);
    endLabel.setLookAndFeel(nullptr);
}

void MappingEditor::resized()
{
    Rectangle<int> area = getLocalBounds();
    int w = getWidth();
    int h = getHeight();
    
    area.reduce(w*0.1f, h*0.1f);
    area.removeFromTop(h*0.2f);
    operationSelect.setBounds(area.removeFromTop(h*0.1f));
    area.removeFromTop(h*0.1f);
    Rectangle<int> startArea = area.removeFromTop(h*0.1f);
    startLabel.setBounds(startArea.removeFromLeft(w*0.1f));
    startSlider.setBounds(startArea);
    area.removeFromTop(h*0.02f);
    Rectangle<int> endArea = area.removeFromTop(h*0.1f);
    endLabel.setBounds(endArea.removeFromLeft(w*0.1f));
    endSlider.setBounds(endArea);
}

void MappingEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &startSlider)
    {
        hook.min = startSlider.getValue();
    }
    else if (slider == &endSlider)
    {
        hook.max = endSlider.getValue();
    }
    slider->setSkewFactorFromMidPoint(0.0f);
}

void MappingEditor::buttonClicked(juce::Button* button)
{
    
}

void MappingEditor::comboBoxChanged (ComboBox *comboBox)
{
    if (comboBox == &operationSelect)
    {
        hook.operation = (HookOperation)(operationSelect.getSelectedId()-1);
    }
}

MappingSource& MappingEditor::getSource()
{
    return source;
}

MappingTarget& MappingEditor::getTarget()
{
    return target;
}
