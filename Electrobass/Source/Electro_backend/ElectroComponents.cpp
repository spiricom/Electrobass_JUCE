/*
  ==============================================================================

    ElectroComponents.cpp
    Created: 19 Feb 2021 12:42:05pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "ElectroComponents.h"
#include "../PluginEditor.h"

//==============================================================================
//==============================================================================

MappingSource::MappingSource(ElectroAudioProcessorEditor& editor, MappingSourceModel& m,
                             const String &displayName) :
Component(m.name),
label(displayName, displayName),
button(displayName, DrawableButton::ButtonStyle::ImageFitted),
processor(editor.processor),
model(m)
{
    addMouseListener(&editor, true);
    
    label.setJustificationType(Justification::centredLeft);
    label.setColour(Label::outlineColourId, model.colour);
    label.setColour(Label::textColourId, model.colour);
    addAndMakeVisible(&label);
    
    image = Drawable::createFromImageData(BinaryData::mappingsourceicon_svg,
                                          BinaryData::mappingsourceicon_svgSize);
    button.setImages(image.get());
    addAndMakeVisible(&button);
}

MappingSource::MappingSource(ElectroAudioProcessorEditor& editor, MappingSourceModel& m) :
Component(m.name),
label(m.name, m.name),
button(m.name, DrawableButton::ButtonStyle::ImageFitted),
processor(editor.processor),
model(m)
{
    addMouseListener(&editor, true);
    
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

MappingTarget::MappingTarget(ElectroAudioProcessorEditor& editor, MappingTargetModel& m) :
Slider(m.name),
processor(editor.processor),
model(m),
text(""),
sliderEnabled(false),
removable(true)
{    
    setDoubleClickReturnValue(true, 0.);
    setSliderStyle(SliderStyle::LinearBarVertical);
    setTextBoxIsEditable(false);
    setVelocityBasedMode(true);
    setVelocityModeParameters(1.0, 1, 0.075);
    setColour(trackColourId, Colours::black);
    setColour(backgroundColourId, Colours::black);
    setTextColour(Colours::transparentBlack);
    updateText();
    
    model.onMappingChange = [this](bool directChange, bool sendListenerNotif) {
        // Handle range and overflow saving and keeps the
        // target slider (the box that you click and drag)
        // in sync with the actual mapping value
        update(directChange, sendListenerNotif);
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
    if (model.currentSource == nullptr) setMapping(source, 0.f);
    else setMappingScalar(source);
}

void MappingTarget::paint(Graphics& g)
{
    Slider::paint(g);
    
//    if (model.currentScalarSource != nullptr)
//    {
//        g.setColour(model.currentScalarSource->colour);
        
//        Justification just =
//        model.currentScalarSource == nullptr ? Justification::centred : Justification::centredLeft;
//        Label* label = getValueLabel();
//        label->setJustificationType(just);
//        label->setBorderSize(BorderSize<int>(0, 1, 0, 0));
        
//        int x = (label->getWidth()/2) + 2;
//        int w = x - 4;
//        g.drawFittedText(getScalarString(), x, 4, w, w*0.7, Justification::centred, 1);
//        g.fillEllipse(getWidth()-5, 3, 3, 3);
//    }
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
            if (removable) {
                PopupMenu menu;
                menu.addItem(1, "Remove");
                if (model.currentScalarSource != nullptr)
                {
                    menu.addItem(2, "Remove Scalar (" + String(model.currentScalarSource->name) + ")");
                }
                menu.showMenuAsync(PopupMenu::Options(),
                                   ModalCallbackFunction::forComponent (menuCallback, this) );
            }
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

void MappingTarget::update(bool directChange, bool sendListenerNotif)
{
    ElectroDial* dial = dynamic_cast<ElectroDial*>(getParentComponent());
    Slider& main = dial->getSlider();
    
    auto value = main.getValue();
    auto min = main.getMinimum();
    auto max = main.getMaximum();
    auto interval = main.getInterval();
    auto skew = main.getSkewFactor();
    setSkewFactor(skew);
    
    if (model.currentSource != nullptr)
    {
        // For initialization and when range is set directly by the target slider
        // as opposed to by the parent dial, which require additional handling
        if (directChange)
        {
            setRange(min-value, max-value, interval);
            setValue(model.end, dontSendNotification);
            lastProportionalValue = valueToProportionOfLength(getValue());
            lastProportionalParentValue = main.valueToProportionOfLength(main.getValue());
        }
        else
        {
            auto proportionalParentDelta =
            main.valueToProportionOfLength(main.getValue()) - lastProportionalParentValue;
            overflowValue = lastProportionalValue + proportionalParentDelta;
            
            lastProportionalValue = overflowValue;
            lastProportionalParentValue = main.valueToProportionOfLength(main.getValue());
            
            setRange(min-value, max-value, interval);
            
            model.setMappingRange(proportionOfLengthToValue(jlimit(0., 1., overflowValue)),
                                  false, false, false);
        }
        
        sliderEnabled = true;
        String name = model.currentSource->name;
        setTextColour(model.currentSource->colour);
        if ((name.substring(0, 1)=="F") || (name.substring(0, 1)=="K"))
        {
            setText(name.substring(0, 2));
        }
        else if (name.getTrailingIntValue() > 0) setText(String(name.getTrailingIntValue()));
        else setText(name.substring(0, 1));

        setValue(model.end, sendListenerNotif ? sendNotificationAsync : dontSendNotification);
    }
    else
    {
        sliderEnabled = false;
        setTextColour(Colours::transparentBlack);
        setText("");
        lastProportionalValue = 0;
        // Guarantee a change event is sent to listeners and set to 0
        // Feels like there should be a better way to do this...
        setValue(0.f, sendListenerNotif ? sendNotificationAsync : dontSendNotification);
        //        getParentComponent()->repaint();
    }
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

void MappingTarget::setMappingRange(float end, bool directChange, bool sendListenerNotif)
{
    model.setMappingRange(end, true, directChange, sendListenerNotif);
}

void MappingTarget::setMappingScalar(MappingSource* source)
{
    if(removable)
        model.setMappingScalar(&source->getModel(), true);
    // The parent dial draw some stuff based on this so we'll repaint
    getParentComponent()->repaint();
}

void MappingTarget::removeMapping()
{
    model.removeMapping(true);
    // The parent dial draw some stuff based on this so we'll repaint
    getParentComponent()->repaint();
}

void MappingTarget::removeScalar()
{
    model.removeScalar(true);
    // The parent dial draw some stuff based on this so we'll repaint
    getParentComponent()->repaint();
}

Label* MappingTarget::getValueLabel()
{
    // Kind of a hack; find the child label of this slider so we can set it's justification
    // The label is not otherwise accessible
    for (auto child : getChildren())
    {
        if (auto* label = dynamic_cast<Label*> (child))
        {
            return label;
        }
    }
    return nullptr;
}

String MappingTarget::getScalarString()
{
    String text = String();
    if (model.currentScalarSource != nullptr)
    {
        int trailing = model.currentScalarSource->name.getTrailingIntValue();
        if (trailing > 0) text = String(trailing);
        else text = model.currentScalarSource->name.substring(0, 1);
    }
    return text;
}

void MappingTarget::menuCallback(int result, MappingTarget* target)
{
    if (result == 1)
    {
        target->removeMapping();
    }
    else if (result == 2)
    {
        target->removeScalar();
    }
}

//==============================================================================
//==============================================================================
ElectroDial::ElectroDial(ElectroAudioProcessorEditor& editor, const String& paramName, const String& displayName, bool isSource, bool isTarget) :
label(displayName, displayName),
paramName(paramName),
editor(editor)
{
    setName(paramName);
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 4, 4);
    slider.setRange(0., 1.);
    slider.addListener(this);
    slider.setName(paramName);
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
        addAndMakeVisible(&label);
        
        for (int i = 0; i < numTargets; ++i)
        {
            t.add(new MappingTarget(editor, *editor.processor.getMappingTarget(paramName + " T" + String(i+1))));
            // calling sliderValueChanged directly from the parent module instead
            // so we can ensure ordering
//            t[i]->addListener(this);
            t[i]->addMouseListener(this, true);
            addAndMakeVisible(t[i]);
        }
    }
    else
    {
        label.setJustificationType(Justification::centred);
        addAndMakeVisible(&label);
    }
}

ElectroDial::~ElectroDial()
{
    slider.setLookAndFeel(nullptr);
    label.setLookAndFeel(nullptr);
}

void ElectroDial::paint(Graphics& g)
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
        
        if (!t[i]->isActive()) continue;
        
        String text = t[i]->getScalarString();
        if (text.isNotEmpty())
        {
            int w = t[i]->getWidth()*0.55f;
            int h = t[i]->getHeight()*0.55f;
            int x = t[i]->getX();
            if (i == 1) x += (t[i]->getWidth()-w)/2;
            else if (i == 2) x += t[i]->getWidth()-w-1;
            int y = t[i]->getY()-h;
            
            // Draw a little box on top of
            g.setColour(Colours::grey);
            g.drawVerticalLine(x, y, t[i]->getY());
            g.drawVerticalLine(x+w, y, t[i]->getY());
            g.drawHorizontalLine(y, x, x+w);
            
            g.setFont(ElectroLookAndFeel::getInstance()->getPopupMenuFont().withHeight(h));
            g.setColour(t[i]->getScalarColour());
            g.drawFittedText(text, x+1, y+1, w-1, h-1,
                             Justification::centred, 1);
        }

		Rectangle<int> inner = slider.getBounds().expanded(ringWidth * (i - 1) + 1,
														   ringWidth * (i - 1) + 1);

		x = inner.getX();
		y = inner.getY();
		width = inner.getWidth();
		height = inner.getHeight();

		radius = jmin(width / 2, height / 2) - width * 0.15f;
		centreX = x + width * 0.5f;
		centreY = y + height * 0.5f;
		auto rx2 = centreX - radius;
		auto ry2 = centreY - radius;
		auto rw2 = radius * 2.0f;
		auto b2 = rw2 * 0.04f;
        
        auto sliderNorm = slider.valueToProportionOfLength(slider.getValue());
        auto targetNorm = t[i]->valueToProportionOfLength(t[i]->getValue()) - sliderNorm;
        
        auto currentAngle = startAngle + (sliderNorm * (endAngle - startAngle));
        auto angle = currentAngle + (targetNorm * (endAngle - startAngle));
        angle = fmax(startAngle, fmin(angle, endAngle));

        if (currentAngle != angle)
        {
            Path arc;
            arc.addArc(rx - b * 4, ry - b * 4, rw + b * 8, rw + b * 8, currentAngle, angle, true);

            // Easiest way to find the point we want to draw to is to make a separate path and get it's start
            Path arc2;
            arc2.addArc(rx2 - b2 * 4, ry2 - b2 * 4, rw2 + b2 * 8, rw2 + b2 * 8, angle, currentAngle, true);
            arc.lineTo(arc2.getPointAlongPath(0));

            arc.addArc(rx2 - b2 * 4, ry2 - b2 * 4, rw2 + b2 * 8, rw2 + b2 * 8, angle, currentAngle, false);
            arc.lineTo(arc.getPointAlongPath(0));

            g.setColour(t[i]->getColour());
            g.fillPath(arc);
        }
        
        if (t[i]->isBipolar())
        {
            angle = currentAngle - (targetNorm * (endAngle - startAngle));
            currentAngle = fmax(startAngle, fmin(currentAngle, endAngle));
            angle = fmax(startAngle, fmin(angle, endAngle));

            if (currentAngle != angle)
            {
                Path oppArc;
                oppArc.addArc(rx - b * 4, ry - b * 4, rw + b * 8, rw + b * 8, currentAngle, angle, true);

                Path oppArc2;
                oppArc2.addArc(rx2 - b2 * 4, ry2 - b2 * 4, rw2 + b2 * 8, rw2 + b2 * 8, angle, currentAngle, true);
                oppArc.lineTo(oppArc2.getPointAlongPath(0));

                oppArc.addArc(rx2 - b2 * 4, ry2 - b2 * 4, rw2 + b2 * 8, rw2 + b2 * 8, angle, currentAngle, false);
                oppArc.lineTo(oppArc.getPointAlongPath(0));

                g.setColour(t[i]->getColour().withSaturation(0.1));
                g.fillPath(oppArc);
            }
        }
    }
}

void ElectroDial::resized()
{
    Rectangle<int> area = getLocalBounds();
    int w = area.getWidth();
    int h = area.getHeight();
    
    Rectangle<int> labelArea = area.removeFromTop(h * 0.225f);
    if (s == nullptr) label.setBounds(labelArea);
    else s->setBounds(labelArea);
    
    if (t.isEmpty())
    {
        slider.setBounds(area.removeFromTop(area.getWidth()));
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

void ElectroDial::mouseDown(const MouseEvent& event)
{
//    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(event.originalComponent->getParentComponent()))
//    {
//        mt->updateRange();
//    }
}

void ElectroDial::sliderValueChanged(Slider* s)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(s))
    {
        mt->setMappingRange(mt->getValue(), true, true);
    }
    else
    {
        if (lastSliderValue != DBL_MAX)
        {
            if (slider.getSkewFactor() != 1.)
            {
                // Get the proportional change of the main slider
                auto pd = slider.valueToProportionOfLength(slider.getValue()) -
                slider.valueToProportionOfLength(lastSliderValue);
                for (auto mt : t)
                {
                    if (!mt->isActive()) continue;
                    // Add it to the proportional value of the targets
                    auto mtp = mt->valueToProportionOfLength(mt->getValue()) + pd;
                    if (0. <= mtp && mtp <= 1.)
                    {
                        // Take the real value of the new proportional value and subtract the real change of the main slider
                        mt->setMappingRange(mt->proportionOfLengthToValue(mtp)-(slider.getValue()-lastSliderValue), false, false);
                    }
                }
            }
            else
            {
                for (auto mt : t) mt->setMappingRange(mt->getValue(), false, false);
            }
        }
        lastSliderValue = slider.getValue();
    }
    repaint();
}

void ElectroDial::setRange(double newMin, double newMax, double newInt)
{
    slider.setRange(newMin, newMax, newInt);
}



void ElectroDial::setValue(double val)
{
    slider.setValue(val);
}

void ElectroDial::setValueWithoutNotifyingHost(double val)
{
    slider.setValue(val,dontSendNotification);
}

void ElectroDial::setValueNotif(double val, NotificationType notif)
{
    slider.setValue(val,notif);
}
void ElectroDial::setText (const String& newText, NotificationType notification)
{
    label.setText(newText, notification);
}

void ElectroDial::setFont (const Font& newFont)
{
    label.setFont(newFont);
}

MappingTarget* ElectroDial::getTarget(int index)
{
    return t[index];
}

OwnedArray<MappingTarget>& ElectroDial::getTargets()
{
    return t;
}

MappingSource* ElectroDial::getSource()
{
    return s.get();
}


void TuningTab::textEditorFocusLost (TextEditor&)
{
    
}

void TuningTab::textEditorEscapeKeyPressed (TextEditor&)
{
    
}

void TuningTab::buttonClicked (Button *b)
{
    if(b == &importButton)
    {
        importScala();
    }
    else if (b == &resetButton)
    {
        sclTextEditor.setText(currentScalaString = processor.tuner.resetSCL(processor.centsDeviation));
        kbmTextEditor.setText(currentKBMString = processor.tuner.resetKBM(processor.centsDeviation));
        processor.tuner.setIsMTS(false);
        MTSButton.setToggleState(false, dontSendNotification);
    }
    else if (b == &importKBMButton)
    {
        importKBM();
    }
    else if (b == &sendTuningButton)
    {
        processor.sendTuningMidiMessage(tuningNameEditor.getText(), tuningNumber.getValue());
    }
    else if (b==& sendOpenStrings)
    {
        processor.sendOpenStringMidiMessage();
    }
}
