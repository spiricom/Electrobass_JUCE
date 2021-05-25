/*
  ==============================================================================

    ESComponents.cpp
    Created: 19 Feb 2021 12:42:05pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "ESComponents.h"
#include "PluginEditor.h"

//==============================================================================
//==============================================================================

MappingSource::MappingSource(ESAudioProcessorEditor& editor, const String &name, float* source,
                             int n, bool bipolar, Colour colour) :
Component(name),
label(name, name),
button(name, DrawableButton::ButtonStyle::ImageFitted),
source(source),
numSourcePointers(n),
bipolar(bipolar),
colour(colour)
{
    addMouseListener(&editor, true);
    
    label.setLookAndFeel(&laf);
    label.setJustificationType(Justification::centredLeft);
    label.setColour(Label::outlineColourId, colour);
    label.setColour(Label::textColourId, colour);
    addAndMakeVisible(&label);
    
    image = Drawable::createFromImageData(BinaryData::mappingsourceicon_svg,
                                          BinaryData::mappingsourceicon_svgSize);
    button.setImages(image.get());
    addAndMakeVisible(&button);
}

MappingSource::~MappingSource()
{
    source = nullptr;
    label.setLookAndFeel(nullptr);
}

void MappingSource::resized()
{
    Rectangle<int> area = getLocalBounds();
    label.setBounds(area);
    button.setBounds(area.removeFromRight(getHeight()));
}

float* MappingSource::getValuePointer()
{
    return source;
}

int MappingSource::getNumSourcePointers()
{
    return numSourcePointers;
}

//==============================================================================
//==============================================================================

MappingTarget::MappingTarget(const String &name, OwnedArray<SmoothedParameter>& target, int index) :
Slider(name),
text(""),
target(target),
index(index),
sliderEnabled(false),
colour(Colours::transparentBlack)
{
    setLookAndFeel(&laf);
    setRange(-1., 1.);
    setSliderStyle(SliderStyle::LinearBarVertical);
    setTextBoxIsEditable(false);
    setVelocityBasedMode(true);
    setVelocityModeParameters(1.0, 1, 0.08);
    setColour(trackColourId, Colours::black);
    setColour(backgroundColourId, Colours::black);
    setColour(textBoxTextColourId, colour);
    updateText();
}

MappingTarget::~MappingTarget()
{
    setLookAndFeel(nullptr);
}

bool MappingTarget::isInterestedInDragSource(const SourceDetails &dragSourceDetails)
{
    return (dynamic_cast<MappingSource*>(dragSourceDetails.sourceComponent.get()));
}

void MappingTarget::itemDropped(const SourceDetails &dragSourceDetails)
{
    MappingSource* source = dynamic_cast<MappingSource*>(dragSourceDetails.sourceComponent.get());
    setMapping(source, 0.f, HookAdd);
}

void MappingTarget::resized()
{
    Slider::resized();
}

void MappingTarget::mouseDown(const MouseEvent& event)
{
    if (sliderEnabled) Slider::mouseDown(event);
    if (event.mods.isCtrlDown() || event.mods.isRightButtonDown())
    {
        PopupMenu menu;
        menu.setLookAndFeel(&laf);
        menu.addItem(1, "Remove");        
        menu.showMenuAsync(PopupMenu::Options(),
                           ModalCallbackFunction::forComponent (menuCallback, this) );
    }
}

void MappingTarget::mouseDrag(const MouseEvent& event)
{
    if (sliderEnabled)
    {
        Slider::mouseDrag(event);
    }
}

void MappingTarget::setText(String s)
{
    text = s;
    updateText();
}

void MappingTarget::setTextColour(Colour c)
{
    colour = c;
    setColour(textBoxTextColourId, colour);
}

void MappingTarget::setMapping(MappingSource* source, float end, HookOperation op)
{
    if (source == nullptr) return;
    sliderEnabled = true;
    String name = source->getName();
    setTextColour(source->getColour());
    setText(String(name.getTrailingIntValue()));
    bipolar = source->isBipolar();
    
    int i = 0;
    int n = source->getNumSourcePointers();
    float start = bipolar ? -end : 0.f;
    for (auto t : target)
    {
        t->setHook(index, &(source->getValuePointer()[i]), start, end, op);
        if (i < n-1) i++;
    }
    ESDial* attached = dynamic_cast<ESDial*>(getParentComponent());
    setValue(end / attached->getSlider().getRange().getLength());
}

void MappingTarget::removeMapping()
{
    sliderEnabled = false;
    setTextColour(Colours::transparentBlack);
    setText("");
    for (auto t : target)
    {
        t->resetHook(index);
    }
    getParentComponent()->repaint();
    setValue(0.0);
}

void MappingTarget::setMappingRange(float end)
{
    float start = bipolar ? -end : 0.f;
    for (auto t : target)
    {
        t->setRange(index, start, end);
    }
    ESDial* attached = dynamic_cast<ESDial*>(getParentComponent());
    setValue(end / attached->getSlider().getRange().getLength());
    DBG(String(start) + " " + String(end));
}

void MappingTarget::menuCallback(int result, MappingTarget* target)
{
    if (result == 1)
    {
        target->removeMapping();
    }
}

//==============================================================================
//==============================================================================

ESDial::ESDial(ESAudioProcessorEditor& editor, const String& name) :
label(name, name)
{
    slider.setLookAndFeel(&laf);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 4, 4);
    slider.setRange(0., 1.);
    slider.addListener(this);
    addAndMakeVisible(&slider);
    
    label.setJustificationType(Justification::centred);
    label.setLookAndFeel(&laf);
    addAndMakeVisible(&label);
}

ESDial::ESDial(ESAudioProcessorEditor& editor, const String& name,
               Colour colour, float* source, int n, bool bipolar) :
label(name, name)
{
    slider.setLookAndFeel(&laf);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 4, 4);
    slider.setRange(0., 1.);
    slider.addListener(this);
    addAndMakeVisible(&slider);

    s = std::make_unique<MappingSource>(editor, name, source, n, bipolar, colour);
    addAndMakeVisible(s.get());
}

ESDial::ESDial(ESAudioProcessorEditor& editor, const String& name,
               OwnedArray<SmoothedParameter>& target) :
label(name, name)
{
    slider.setLookAndFeel(&laf);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 4, 4);
    slider.setRange(0., 1.);
    slider.addListener(this);
    addAndMakeVisible(&slider);

    label.setJustificationType(Justification::centred);
    label.setBorderSize(BorderSize<int>(0));
    label.setLookAndFeel(&laf);
    addAndMakeVisible(&label);

    for (int i = 0; i < numTargets; ++i)
    {
        t.add(new MappingTarget("T" + String(i+1), target, i));
        t[i]->addListener(this);
        t[i]->addMouseListener(this, true);
        addAndMakeVisible(t[i]);
    }
}

ESDial::~ESDial()
{
    slider.setLookAndFeel(nullptr);
    label.setLookAndFeel(nullptr);
}

void ESDial::paint(Graphics& g)
{
    Rectangle<int> area = getLocalBounds();
    int w = area.getWidth();
    int h = area.getHeight();
    area.removeFromTop(h * 0.225f);
    
    if (!t.isEmpty())
    {
        int ringWidth = h * 0.05f;
        for (int i = 0; i < t.size(); ++i)
        {
            Rectangle<int> outer = slider.getBounds().expanded(ringWidth * (i), ringWidth * (i));
            
            int x = outer.getX();
            int y = outer.getY();
            int width = outer.getWidth();
            int height = outer.getHeight();
            
            auto radius = jmin(width / 2, height / 2) - width*0.15f;
            auto centreX = x + width * 0.5f;
            auto centreY = y + height * 0.5f;
            auto rx = centreX - radius;
            auto ry = centreY - radius;
            auto rw = radius * 2.0f;
            auto b = rw * 0.05f;
            
            auto startAngle = slider.getRotaryParameters().startAngleRadians;
            auto endAngle = slider.getRotaryParameters().endAngleRadians;
            auto currentAngle = startAngle +
            slider.valueToProportionOfLength(slider.getValue()) * (endAngle - startAngle);
            auto angle = currentAngle + (t[i]->getValue() * (endAngle - startAngle));
            angle = fmax(startAngle, fmin(angle, endAngle));
            
            Path arc;
            arc.addArc(rx - b*4, ry - b*4, rw + b*8, rw + b*8, currentAngle, angle, true);
            
            Rectangle<int> inner = slider.getBounds().expanded(ringWidth * (i-1)+1, ringWidth * (i-1)+1);
            
            x = inner.getX();
            y = inner.getY();
            width = inner.getWidth();
            height = inner.getHeight();
            
            radius = jmin(width / 2, height / 2) - width*0.15f;
            centreX = x + width * 0.5f;
            centreY = y + height * 0.5f;
            auto rx2 = centreX - radius;
            auto ry2 = centreY - radius;
            auto rw2 = radius * 2.0f;
            auto b2 = rw * 0.05f;

            arc.addArc(rx2 - b2*4, ry2 - b*4, rw2 + b*8, rw2 + b2*8, angle, currentAngle, false);
            g.setColour(t[i]->getColour());
            g.fillPath(arc);
            
            if (t[i]->isBipolar())
            {
                angle = currentAngle - (t[i]->getValue() * (endAngle - startAngle));
                Path oppArc;
                oppArc.addArc(rx - b*4, ry - b*4, rw + b*8, rw + b*8, currentAngle, angle, true);
                oppArc.addArc(rx2 - b2*4, ry2 - b2*4, rw2 + b2*8, rw2 + b2*8, angle, currentAngle, false);
                g.setColour(t[i]->getColour().withSaturation(0.1));
                g.fillPath(oppArc);
            }
        }
    }
}

void ESDial::resized()
{
    Rectangle<int> area = getLocalBounds();
    int w = area.getWidth();
    int h = area.getHeight();
    
    Rectangle<int> labelArea = area.removeFromTop(h * 0.225f);
    if (s == nullptr) label.setBounds(labelArea);
    else s->setBounds(labelArea);
    
    if (t.isEmpty())
    {
        slider.setBounds(area);
    }
    else
    {
        slider.setBounds(area.removeFromTop(h * 0.6f)
                         .withSizeKeepingCentre(h * 0.6f, h * 0.6f)
                         .reduced(h * 0.09f, h * 0.09f));
        area.removeFromTop(h * 0.025f);
        
        int r = area.getWidth() % 3;
        int m = 1;
        if (r == 1) area.removeFromLeft(1);
        if (r == 2) m = 0;
        t[0]->setBounds(area.removeFromLeft(w/3 - m));
        area.removeFromLeft(1);
        t[1]->setBounds(area.removeFromLeft(w/3 - m));
        area.removeFromLeft(1);
        t[2]->setBounds(area.removeFromLeft(w/3 - m));
    }
}

void ESDial::mouseDown(const MouseEvent& event)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(event.originalComponent))
    {
        float pos = slider.valueToProportionOfLength(slider.getValue());
        mt->setRange(-pos, 1.f-pos);
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(event.originalComponent->getParentComponent()))
    {
        float pos = slider.valueToProportionOfLength(slider.getValue());
        mt->setRange(-pos, 1.f-pos);
    }
}

void ESDial::sliderValueChanged(Slider* s)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(s))
    {
        float v = mt->getValue();
        mt->setMappingRange(v*slider.getRange().getLength());
    }
    repaint();
}

void ESDial::setText (const String& newText, NotificationType notification)
{
    label.setText(newText, notification);
}

void ESDial::setFont (const Font& newFont)
{
    label.setFont(newFont);
}

MappingTarget* ESDial::getTarget(int index)
{
    return t[index];
}

MappingSource* ESDial::getSource()
{
    return s.get();
}
