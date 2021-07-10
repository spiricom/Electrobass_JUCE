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

MappingSource::MappingSource(ESAudioProcessorEditor& editor, MappingSourceModel& m,
                             const String &displayName) :
Component(m.name),
label(displayName, displayName),
button(displayName, DrawableButton::ButtonStyle::ImageFitted),
processor(editor.processor),
model(m)
{
    addMouseListener(&editor, true);
    
    label.setLookAndFeel(&laf);
    label.setJustificationType(Justification::centredLeft);
    label.setColour(Label::outlineColourId, model.colour);
    label.setColour(Label::textColourId, model.colour);
    addAndMakeVisible(&label);
    
    image = Drawable::createFromImageData(BinaryData::mappingsourceicon_svg,
                                          BinaryData::mappingsourceicon_svgSize);
    button.setImages(image.get());
    addAndMakeVisible(&button);
}

MappingSource::~MappingSource()
{
    label.setLookAndFeel(nullptr);
}

void MappingSource::resized()
{
    Rectangle<int> area = getLocalBounds();
    label.setBounds(area);
    button.setBounds(area.removeFromRight(getHeight()));
}

//==============================================================================
//==============================================================================

MappingTarget::MappingTarget(ESAudioProcessorEditor& editor, MappingTargetModel& m) :
Slider(m.name),
processor(editor.processor),
model(m),
text(""),
sliderEnabled(false)
{    
    setLookAndFeel(&laf);
    setDoubleClickReturnValue(true, 0.);
    setSliderStyle(SliderStyle::LinearBarVertical);
    setTextBoxIsEditable(false);
    setVelocityBasedMode(true);
    setVelocityModeParameters(1.0, 1, 0.075);
    setColour(trackColourId, Colours::black);
    setColour(backgroundColourId, Colours::black);
    setTextColour(Colours::transparentBlack);
    updateText();
    
    model.onMappingChange = [this](bool sendChangeEvent) {
        updateRange();
        updateValue(sendChangeEvent);
    };
}

MappingTarget::~MappingTarget()
{
    model.onMappingChange = nullptr;
    setLookAndFeel(nullptr);
}

bool MappingTarget::isInterestedInDragSource(const SourceDetails &dragSourceDetails)
{
    return (dynamic_cast<MappingSource*>(dragSourceDetails.sourceComponent.get()));
}

void MappingTarget::itemDropped(const SourceDetails &dragSourceDetails)
{
    MappingSource* source = dynamic_cast<MappingSource*>(dragSourceDetails.sourceComponent.get());
    setMapping(source, 0.f);
}

void MappingTarget::resized()
{
    Slider::resized();
}

void MappingTarget::mouseDown(const MouseEvent& event)
{
    if (sliderEnabled)
    {
        if (event.mods.isCtrlDown() || event.mods.isRightButtonDown())
        {
            PopupMenu menu;
            menu.setLookAndFeel(&laf);
            menu.addItem(1, "Remove");
            menu.showMenuAsync(PopupMenu::Options(),
                               ModalCallbackFunction::forComponent (menuCallback, this) );
        }
        else
        {
            Slider::mouseDown(event);
        }
    }
}

void MappingTarget::mouseDrag(const MouseEvent& event)
{
    if (sliderEnabled)
    {
        Slider::mouseDrag(event);
    }
}

void MappingTarget::updateValue(bool sendChangeEvent)
{
    if (model.currentSource != nullptr)
    {
        sliderEnabled = true;
        String name = model.currentSource->name;
        setTextColour(model.currentSource->colour);
        setText(String(name.getTrailingIntValue()));
        
        setValue(model.value, sendChangeEvent ? sendNotification : dontSendNotification);
    }
    else
    {
        sliderEnabled = false;
        setTextColour(Colours::transparentBlack);
        setText("");
        
        // Guarantee a change event is sent to listeners and set to 0
        // Feels like there should be a better way to do this...
        setValue(0.f, sendChangeEvent ? sendNotification : dontSendNotification);
//        getParentComponent()->repaint();
    }
}

void MappingTarget::updateRange()
{
    ESDial* dial = dynamic_cast<ESDial*>(getParentComponent());
    
    auto value = dial->getSlider().getValue();
    auto min = dial->getSlider().getMinimum();
    auto max = dial->getSlider().getMaximum();
    auto interval = dial->getSlider().getInterval();
    auto skew = dial->getSlider().getSkewFactor();
    
    if (model.isBipolar()) setRange((min-max)*0.5, (max-min)*0.5, interval);
    else setRange(min-value, max-value, interval);
    
    setSkewFactor(skew);
}

void MappingTarget::setText(String s)
{
    text = s;
    updateText();
}

void MappingTarget::setTextColour(Colour c)
{
    setColour(textBoxTextColourId, c);
}

void MappingTarget::setMapping(MappingSource* source, float end)
{
    model.setMapping(&source->getModel(), end, true);
}

void MappingTarget::removeMapping()
{
    model.removeMapping(true);
}

void MappingTarget::setMappingRange(float end, bool sendChangeEvent)
{
    model.setMappingRange(end, sendChangeEvent);
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
ESDial::ESDial(ESAudioProcessorEditor& editor, const String& paramName, const String& displayName, bool isSource, bool isTarget) :
label(displayName, displayName)
{
    slider.setLookAndFeel(&laf);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 4, 4);
    slider.setRange(0., 1.);
    slider.addListener(this);
    addAndMakeVisible(&slider);

    if (isSource)
    {
        s = std::make_unique<MappingSource>
        (editor, *editor.processor.getMappingSource(paramName), displayName);
        addAndMakeVisible(s.get());
    }
    else if (isTarget)
    {
        label.setJustificationType(Justification::centred);
        label.setBorderSize(BorderSize<int>(0));
        label.setLookAndFeel(&laf);
        addAndMakeVisible(&label);
        
        for (int i = 0; i < numTargets; ++i)
        {
            t.add(new MappingTarget(editor, *editor.processor.getMappingTarget(paramName + " T" + String(i+1))));
            t[i]->addListener(this);
            t[i]->addMouseListener(this, true);
            addAndMakeVisible(t[i]);
        }
    }
    else
    {
        label.setJustificationType(Justification::centred);
        label.setLookAndFeel(&laf);
        addAndMakeVisible(&label);
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
    int h = area.getHeight();
    area.removeFromTop(h * 0.225f);
    
    int ringWidth = h * 0.05f;
    for (int i = 0; i <= t.size(); ++i)
    {
        int expand = ringWidth*i;
        if (i == t.size()) expand -= ringWidth/2;
        Rectangle<int> outer = slider.getBounds().expanded(expand, expand);
        
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
        auto b = rw * 0.04f;
        
        auto startAngle = slider.getRotaryParameters().startAngleRadians;
        auto endAngle = slider.getRotaryParameters().endAngleRadians;
        
        if (i == t.size())
        {
            Path marks;
            marks.addArc(rx - b*4, ry - b*4, rw + b*8, rw + b*8, startAngle, endAngle, true);
            float lengths[2];
            lengths[0] = 1.f;
            lengths[1] = 2.f;
            PathStrokeType(h * 0.025f).createDashedStroke(marks, marks, lengths, 2);
            g.setColour(Colours::grey);
            g.fillPath(marks);
            continue;
        }
        
        // Should not reflect negative values
        auto sliderNorm = slider.valueToProportionOfLength(slider.getValue());
        // Should reflect negative values
        auto targetNorm = t[i]->valueToProportionOfLength(t[i]->getValue()) - sliderNorm;
        auto currentAngle = startAngle + (sliderNorm * (endAngle - startAngle));
        auto angle = currentAngle + (targetNorm * (endAngle - startAngle));
        angle = fmax(startAngle, fmin(angle, endAngle));
        
        Path arc;
        arc.addArc(rx - b*4, ry - b*4, rw + b*8, rw + b*8, currentAngle, angle, true);
        
        Rectangle<int> inner = slider.getBounds().expanded(ringWidth*(i-1) + 1, ringWidth*(i-1) + 1);
        
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
        auto b2 = rw2 * 0.04f;
        
        arc.addArc(rx2 - b2*4, ry2 - b2*4, rw2 + b2*8, rw2 + b2*8, angle, currentAngle, false);
        g.setColour(t[i]->getColour());
        g.fillPath(arc);
        
        if (t[i]->isBipolar())
        {
            angle = currentAngle - (targetNorm * (endAngle - startAngle));
            currentAngle = fmax(startAngle, fmin(currentAngle, endAngle));
            angle = fmax(startAngle, fmin(angle, endAngle));
            Path oppArc;
            oppArc.addArc(rx - b*4, ry - b*4, rw + b*8, rw + b*8, currentAngle, angle, true);
            oppArc.addArc(rx2 - b2*4, ry2 - b2*4, rw2 + b2*8, rw2 + b2*8, angle, currentAngle, false);
            g.setColour(t[i]->getColour().withSaturation(0.1));
            g.fillPath(oppArc);
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
                         .reduced(h * 0.11f, h * 0.11f));
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
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(event.originalComponent->getParentComponent()))
    {
        mt->updateRange();
    }
}

void ESDial::sliderValueChanged(Slider* s)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(s))
    {
        mt->setMappingRange(mt->getValue());
    }
    else
    {
        if (lastSliderValue != DBL_MAX && slider.getSkewFactor() != 1.)
        {
            // Get the proportional change of the main slider
            auto pd = slider.valueToProportionOfLength(slider.getValue()) -
            slider.valueToProportionOfLength(lastSliderValue);
            for (auto mt : t)
            {
                // Add it to the proportional value of the targets
                auto mtp = mt->valueToProportionOfLength(mt->getValue()) + pd;
                if (0. <= mtp && mtp <= 1.)
                {
                    // Take the real value of the new proportional value and subtract the real change of the main slider
                    mt->setMappingRange(mt->proportionOfLengthToValue(mtp)-(slider.getValue()-lastSliderValue), false);
                }
            }
        }
        lastSliderValue = slider.getValue();
    }
    repaint();
}

void ESDial::setRange(double newMin, double newMax, double newInt)
{
    slider.setRange(newMin, newMax, newInt);
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

OwnedArray<MappingTarget>& ESDial::getTargets()
{
    return t;
}

MappingSource* ESDial::getSource()
{
    return s.get();
}
