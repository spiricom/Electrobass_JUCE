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
removable(true),
text(""),
sliderEnabled(false)
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

void MappingTarget::setOutlineColour(Colour c)
{
    setColour(textBoxOutlineColourId, c);
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
paramName(paramName),
editor(editor),
label(displayName, displayName)
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
            t[i]->setOutlineColour (juce::Colours::darkslategrey);
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
        t[0]->setBounds(area.removeFromLeft(w/3 - m) * .98f);
        area.removeFromLeft(1);
        t[1]->setBounds(area.removeFromLeft(w/3 - m)* .98f);
        area.removeFromLeft(1);
        t[2]->setBounds(area.removeFromLeft(w/3 - m)* .98f);
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
        
        for(auto inputs : MidiOutput::getAvailableDevices())
        {
            if (inputs.name == "Electrosteel")
            {
                
                
                editor.sysexOut = MidiOutput::openDevice(inputs.identifier);
                
                
            }
        }
        if (editor.sysexOut )
        {
            MidiBuffer midiMessages;
            float tuningNumber_ = tuningNumber.getValue();
            String presetName = tuningNameEditor.getText();
            uint16_t currentChunk = 0;
            Array<uint8_t> data7bitInt;
            union uintfUnion fu;
            
            data7bitInt.add(1); // saying it's a preset
            data7bitInt.add(tuningNumber_); // which preset are we saving
            
            for (int i = 0; i < presetName.length(); i++)
            {
                data7bitInt.add((presetName.toUTF8()[i] & 127)); //printable characters are in the 0-127 range
                
            }
            uint16 remainingBlanks = 14 - presetName.length();
            for (uint16 i = 0; i < remainingBlanks; i++)
            {
                data7bitInt.add(32);
            }
            //MidiMessage presetMessage = ;
            
            midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
            
            currentChunk++;
            Array<float> data;
            for (int i = 0; i < 128; i++)
            {
                data.add(processor.centsDeviation[i]);
            }
            
            
            
            uint16_t currentDataPointer = 0;
            uint16_t sizeOfSysexChunk = (64 / 5) - 3;
            int dataToSend = data.size();
            while(currentDataPointer < dataToSend)
            {
                data7bitInt.clear();
                
                data7bitInt.add(1); // saying it's a tuning
                data7bitInt.add(tuningNumber_); // which tuning are we saving
                
                //data7bitInt.add(currentChunk); // whichChhunk
                uint16_t toSendInThisChunk;
                uint16_t dataRemaining = dataToSend - currentDataPointer;
                if (dataRemaining < sizeOfSysexChunk)
                {
                    toSendInThisChunk = dataRemaining;
                }
                else
                {
                    toSendInThisChunk = sizeOfSysexChunk;
                }
                
                for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
                {
                    fu.f = data[i];
                    data7bitInt.add((fu.i >> 28) & 15);
                    data7bitInt.add((fu.i >> 21) & 127);
                    data7bitInt.add((fu.i >> 14) & 127);
                    data7bitInt.add((fu.i >> 7) & 127);
                    data7bitInt.add(fu.i & 127);
                    
                }
                currentDataPointer = currentDataPointer + toSendInThisChunk;
                MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
                
                midiMessages.addEvent(presetMessage, 0);
                
                currentChunk++;
            }
            data7bitInt.clear();
            data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
            data7bitInt.add(tuningNumber_); // which tuning did we just finish
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
            midiMessages.addEvent(presetMessage, 0);
            editor.sysexOut->sendBlockOfMessagesNow(midiMessages);
        }
    }
    else if (b==& sendOpenStrings)
    {
        for(auto inputs : MidiOutput::getAvailableDevices())
        {
            if (inputs.name == "Electrosteel")
            {
                
                
                editor.sysexOut = MidiOutput::openDevice(inputs.identifier);
                
                
            }
        }
        if (editor.sysexOut )
        {
            MidiBuffer midiMessages;
            uint16_t currentChunk = 0;
            Array<uint8_t> data7bitInt;
            union uintfUnion fu;
            
            data7bitInt.add(3); // saying it's a preset
            data7bitInt.add(0); // which preset are we saving
            
            
            
            
            Array<float> data;
            for (int i = 0; i < 4; i++)
            {
                data.add(processor.openStrings[i]);
            }
            
            
            
            uint16_t currentDataPointer = 0;
            uint16_t sizeOfSysexChunk = (64 / 5) - 3;
            int dataToSend = data.size();
            while(currentDataPointer < dataToSend)
            {
                data7bitInt.clear();
                
                data7bitInt.add(3); // saying it's a tuning
                data7bitInt.add(0); // which tuning are we saving
                
                //data7bitInt.add(currentChunk); // whichChhunk
                uint16_t toSendInThisChunk;
                uint16_t dataRemaining = dataToSend - currentDataPointer;
                if (dataRemaining < sizeOfSysexChunk)
                {
                    toSendInThisChunk = dataRemaining;
                }
                else
                {
                    toSendInThisChunk = sizeOfSysexChunk;
                }
                
                for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
                {
                    fu.f = data[i];
                    data7bitInt.add((fu.i >> 28) & 15);
                    data7bitInt.add((fu.i >> 21) & 127);
                    data7bitInt.add((fu.i >> 14) & 127);
                    data7bitInt.add((fu.i >> 7) & 127);
                    data7bitInt.add(fu.i & 127);
                    
                }
                currentDataPointer = currentDataPointer + toSendInThisChunk;
                MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
                
                midiMessages.addEvent(presetMessage, 0);
                
                currentChunk++;
            }
            data7bitInt.clear();
            data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
            data7bitInt.add(0); // which tuning did we just finish
            MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
            midiMessages.addEvent(presetMessage, 0);
            editor.sysexOut->sendBlockOfMessagesNow(midiMessages);
        }
    }
}

CopedentTable::CopedentTable(ElectroAudioProcessorEditor& e, ElectroAudioProcessor& p, AudioProcessorValueTreeState& vts) :
    processor(p),
    editor(e),
    copedentArray(processor.copedentArray),
    fundamental(processor.copedentFundamental),
    fundamentalField(*this),
    numberField(*this),
    nameField(*this),
    exportChooser("Export copedent to .xml...",
                  File::getSpecialLocation(File::userDocumentsDirectory),
                  "*.xml"),
    importChooser("Import copedent .xml...",
                  File::getSpecialLocation(File::userDocumentsDirectory),
                  "*.xml")
    {
        for (int i = 0; i < CopedentColumnNil; ++i)
        {
            columnList.add(cCopedentColumnNames[i]);
        }
        
        stringTable.setModel (this);
        stringTable.setColour (ListBox::outlineColourId, Colours::grey);
        stringTable.setOutlineThickness (1);
        
        leftTable.setModel (this);
        leftTable.setColour (ListBox::outlineColourId, Colours::grey);
        leftTable.setOutlineThickness (1);
        
        pedalTable.setModel (this);
        pedalTable.setColour (ListBox::outlineColourId, Colours::grey);
        pedalTable.setOutlineThickness (1);
        
        rightTable.setModel (this);
        rightTable.setColour (ListBox::outlineColourId, Colours::grey);
        rightTable.setOutlineThickness (1);
        
        int i = 0;
        int columnId = 1;
        TableHeaderComponent::ColumnPropertyFlags flags =
        TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable;
        stringTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        rightTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        rightTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);

        addAndMakeVisible (stringTable);
        addAndMakeVisible (leftTable);
        addAndMakeVisible (pedalTable);
        addAndMakeVisible (rightTable);
        
        fundamentalField.setRowAndColumn(0, 0);
        addAndMakeVisible (fundamentalField);
        
        fundamentalLabel.setText("Fundamental", dontSendNotification);
        fundamentalLabel.setJustificationType(Justification::centred);
        addAndMakeVisible (fundamentalLabel);
        
        exportButton.setButtonText("Export .xml");
        exportButton.onClick = [this] { exportXml(); };
        addAndMakeVisible(exportButton);
        
        importButton.setButtonText("Import .xml");
        importButton.onClick = [this] { importXml(); };
        addAndMakeVisible(importButton);
        
        numberLabel.setText("#", dontSendNotification);
        numberLabel.setJustificationType(Justification::centred);
        addAndMakeVisible (numberLabel);
        
        numberField.setRowAndColumn(0, -1);
        addAndMakeVisible (numberField);
        
        nameLabel.setText("Name", dontSendNotification);
        nameLabel.setJustificationType(Justification::centred);
        addAndMakeVisible (nameLabel);
        
        nameField.setRowAndColumn(0, -2);
        addAndMakeVisible (nameField);
        
        sendOutButton.setButtonText("Send copedent via MIDI");
        sendOutButton.onClick = [this] {
            for(auto inputs : MidiOutput::getAvailableDevices())
            {
                if (inputs.name == "Electrosteel")
                {
                    
                    
                    editor.sysexOut = MidiOutput::openDevice(inputs.identifier);
                    
                    
                }
            }
            if (editor.sysexOut )
            {
                MidiBuffer midiMessages;
                Array<float> data;
                
                for (int i = 0; i < copedentArray.size(); ++i)
                {
                    for (auto value : copedentArray.getReference(i))
                    {
                        data.add(value);
                    }
                }
                
                Array<uint8_t> data7bitInt;
                union uintfUnion fu;
                
                uint16_t sizeOfSysexChunk = (64 / 5) - 3;
                int dataToSend = data.size();
                uint16_t currentChunk = 0;
                uint16_t currentDataPointer = 0;
                
                data7bitInt.clear();
                data7bitInt.add(2); // saying it's a copedent
                data7bitInt.add(processor.copedentNumber); // saying which copedent number to store (need this to be a user entered value)
                for (int i = 0; i < processor.copedentName.length(); i++)
                {
                    if (i < 10)
                    {
                        data7bitInt.add((processor.copedentName.toUTF8()[i] & 127)); //printable characters are in the 0-127 range
                    }
                }
                uint16 remainingBlanks = 0;
                if (processor.copedentName.length() < 10)
                {
                    remainingBlanks = 10 - processor.copedentName.length();
                }
                for (uint16 i = 0; i < remainingBlanks; i++)
                {
                    data7bitInt.add(32);
                }
                
                midiMessages.addEvent(MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size()), 0);
                
                currentChunk++;
                
                
                while(currentDataPointer < dataToSend)
                {
                    data7bitInt.clear();
                    
                    data7bitInt.add(2); // saying it's a copedent
                    data7bitInt.add(processor.copedentNumber); // saying which copedent number to store (need this to be a user entered value)
                    //flat7bitInt.add(50 + j);
                    
                    //data7bitInt.add(currentChunk); // whichChhunk
                    uint16_t toSendInThisChunk;
                    uint16_t dataRemaining = dataToSend - currentDataPointer;
                    if (dataRemaining < sizeOfSysexChunk)
                    {
                        toSendInThisChunk = dataRemaining;
                    }
                    else
                    {
                        toSendInThisChunk = sizeOfSysexChunk;
                    }
                    
                    for (int i = currentDataPointer; i < toSendInThisChunk+currentDataPointer; i++)
                    {
                        fu.f = data[i];
                        data7bitInt.add((fu.i >> 28) & 15);
                        data7bitInt.add((fu.i >> 21) & 127);
                        data7bitInt.add((fu.i >> 14) & 127);
                        data7bitInt.add((fu.i >> 7) & 127);
                        data7bitInt.add(fu.i & 127);
                        
                    }
                    currentDataPointer = currentDataPointer + toSendInThisChunk;
                    MidiMessage presetMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
                    
                    midiMessages.addEvent(presetMessage, 0);
                    
                    currentChunk++;
                }
                data7bitInt.clear();
                data7bitInt.add(126); // custom command to start parsing, sysex send is finished!
                data7bitInt.add(processor.copedentNumber); // which copedent did we just finish
                MidiMessage copedentMessage = MidiMessage::createSysExMessage(data7bitInt.getRawDataPointer(), sizeof(uint8_t) * data7bitInt.size());
                midiMessages.addEvent(copedentMessage, 0);
                editor.sysexOut->sendMessageNow(copedentMessage);
            }};
        addAndMakeVisible(sendOutButton);
        addAndMakeVisible(clearButton);
        clearButton.setButtonText("Clear");
        clearButton.onClick = [this] {
            for (int i = 0; i < CopedentColumnNil; ++i)
            {
                for (int v = 0; v < MAX_NUM_VOICES; ++v)
                {
                    copedentArray.getReference(i).set(v, cCopedentArrayInit[i][v]);
                    //refreshComponentForCell(i,v, false, nullptr);
                }
            }
            fundamental = 21.f;
            resized();
            
        };
    }
