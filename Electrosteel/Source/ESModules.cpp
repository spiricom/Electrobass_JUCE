/*
  ==============================================================================

    ESModules.cpp
    Created: 2 Jul 2021 3:06:27pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "ESModules.h"
#include "PluginEditor.h"

//==============================================================================
//==============================================================================
ESComponent::ESComponent() :
outlineColour(Colours::transparentBlack)
{
    setInterceptsMouseClicks(false, true);
}

ESComponent::~ESComponent()
{
    
}

void ESComponent::paint(Graphics &g)
{
    Rectangle<int> area = getLocalBounds();
    
    g.setColour(outlineColour);
    g.drawRect(area);
}

//==============================================================================

ESModule::ESModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                   AudioComponent& ac, float relLeftMargin, float relDialWidth,
                   float relDialSpacing, float relTopMargin, float relDialHeight) :
editor(editor),
vts(vts),
ac(ac),
relLeftMargin(relLeftMargin),
relDialWidth(relDialWidth),
relDialSpacing(relDialSpacing),
relTopMargin(relTopMargin),
relDialHeight(relDialHeight)
{
    String& name = ac.getName();
    StringArray& paramNames = ac.getParamNames();
    for (int i = 0; i < paramNames.size(); i++)
    {
        String paramName = name + " " + paramNames[i];
        String displayName = paramNames[i];
        dials.add(new ESDial(editor, paramName, displayName, false, true));
        addAndMakeVisible(dials[i]);
        sliderAttachments.add(new SliderAttachment(vts, paramName, dials[i]->getSlider()));
        dials[i]->getSlider().addListener(this);
        for (auto t : dials[i]->getTargets())
        {
            t->addListener(this);
            t->addMouseListener(this, true);
            t->update(true, false);
        }
    }
    
    if (ac.isToggleable())
    {
        enabledToggle.addListener(this);
        addAndMakeVisible(enabledToggle);
        buttonAttachments.add(new ButtonAttachment(vts, name, enabledToggle));
    }
}

ESModule::~ESModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void ESModule::resized()
{
    for (int i = 0; i < ac.getParamNames().size(); ++i)
    {
        dials[i]->setBoundsRelative(relLeftMargin + (relDialWidth+relDialSpacing)*i, relTopMargin,
                                    relDialWidth, relDialHeight);
    }
    
    if (ac.isToggleable())
    {
        enabledToggle.setBounds(0, 0, 25, 25);
    }
}

void ESModule::sliderValueChanged(Slider* slider)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ESDial*>(mt->getParentComponent())->sliderValueChanged(slider);
    }
}

void ESModule::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void ESModule::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
}

ESDial* ESModule::getDial (int index)
{
    return dials[index];
}

//==============================================================================
//==============================================================================

OscModule::OscModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ESModule(editor, vts, ac, 0.05f, 0.132f, 0.05f, 0.18f, 0.8f),
chooser("Select wavetable file or folder...",
              File::getSpecialLocation(File::userDocumentsDirectory))
{
    outlineColour = Colours::darkgrey;
    
    // Pitch and freq dials should snap to ints
    getDial(OscPitch)->setRange(-24., 24., 1.);
    getDial(OscFreq)->setRange(-2000., 2000., 1.);
    
    pitchLabel.setLookAndFeel(&laf);
    pitchLabel.setEditable(true);
    pitchLabel.setJustificationType(Justification::centred);
    pitchLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    pitchLabel.addListener(this);
    addAndMakeVisible(pitchLabel);
    
    freqLabel.setLookAndFeel(&laf);
    freqLabel.setEditable(true);
    freqLabel.setJustificationType(Justification::centred);
    freqLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    freqLabel.addListener(this);
    addAndMakeVisible(freqLabel);
    
    displayPitch();
    
    RangedAudioParameter* set = vts.getParameter(ac.getName() + " ShapeSet");
    updateShapeCB();
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    shapeCB.setLookAndFeel(&laf);
    shapeCB.addListener(this);
    shapeCB.addMouseListener(this, true);
    addAndMakeVisible(shapeCB);
//    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.getName() + " ShapeSet", shapeCB));
    
    sendSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    sendSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);
    addAndMakeVisible(sendSlider);
    sliderAttachments.add(new SliderAttachment(vts, ac.getName() + " FilterSend", sendSlider));
    
    f1Label.setText("F1", dontSendNotification);
    f1Label.setJustificationType(Justification::bottomRight);
    f1Label.setLookAndFeel(&laf);
    addAndMakeVisible(f1Label);
    
    f2Label.setText("F2", dontSendNotification);
    f2Label.setJustificationType(Justification::topRight);
    f2Label.setLookAndFeel(&laf);
    addAndMakeVisible(f2Label);
    
    s = std::make_unique<MappingSource>
    (editor, *editor.processor.getMappingSource(ac.getName()), ac.getName());
    addAndMakeVisible(s.get());
}

OscModule::~OscModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void OscModule::resized()
{
    ESModule::resized();
    
    s->setBounds(4, 4, getWidth()*0.1f, enabledToggle.getHeight()-8);
    
    pitchLabel.setBoundsRelative(relLeftMargin+0.5f*relDialWidth,
                                 0.02f, relDialWidth+relDialSpacing, 0.16f);
    
    freqLabel.setBoundsRelative(relLeftMargin+2*relDialWidth+1.5*relDialSpacing,
                                 0.02f, relDialWidth+relDialSpacing, 0.16f);
    
    shapeCB.setBoundsRelative(relLeftMargin+3*(relDialWidth+relDialSpacing), 0.02f,
                              relDialWidth+relDialSpacing, 0.16f);
    
    sendSlider.setBoundsRelative(0.96f, 0.f, 0.04f, 1.0f);
    
    enabledToggle.setBoundsRelative(0.917f, 0.41f, 0.04f, 0.15f);
    
    f1Label.setBoundsRelative(0.9f, 0.05f, 0.06f, 0.15f);
    f2Label.setBoundsRelative(0.9f, 0.80f, 0.06f, 0.15f);
}

void OscModule::sliderValueChanged(Slider* slider)
{
    if (slider == &getDial(OscPitch)->getSlider() ||
        slider == &getDial(OscFine)->getSlider() ||
        slider == &getDial(OscFreq)->getSlider())
    {
        displayPitch();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ESDial*>(mt->getParentComponent())->sliderValueChanged(slider);
        displayPitchMapping(mt);
    }
}

void OscModule::buttonClicked(Button* button)
{
    if (button == &enabledToggle)
    {
        sendSlider.setEnabled(enabledToggle.getToggleState());
        sendSlider.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
    }
}

void OscModule::labelTextChanged(Label* label)
{
    if (label == &pitchLabel)
    {
        auto value = pitchLabel.getText().getDoubleValue();
        int i = value;
        double f = value-i;
        getDial(OscPitch)->getSlider().setValue(i, sendNotificationAsync);
        getDial(OscFine)->getSlider().setValue(f*100., sendNotificationAsync);
    }
    else if (label == &freqLabel)
    {
        auto value = freqLabel.getText().getDoubleValue();
        getDial(OscFreq)->getSlider().setValue(value, sendNotificationAsync);
    }
}

void OscModule::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &shapeCB)
    {
        if (shapeCB.getSelectedItemIndex() == shapeCB.getNumItems()-1)
        {
            chooser.launchAsync (FileBrowserComponent::openMode |
                                 FileBrowserComponent::canSelectFiles |
                                 FileBrowserComponent::canSelectDirectories,
                                 [this] (const FileChooser& chooser)
                                 {
                String path = chooser.getResult().getFullPathName();
                Oscillator& osc = static_cast<Oscillator&>(ac);
                
                if (path.isEmpty())
                {
                    shapeCB.setSelectedItemIndex(0, dontSendNotification);
                    vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(0.);
                    return;
                }
                
                File file(path);
                osc.setWaveTables(file);
                vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(1.);
                updateShapeCB();
            });
        }
        else if (shapeCB.getSelectedItemIndex() >= UserOscShapeSet)
        {
            Oscillator& osc = static_cast<Oscillator&>(ac);
            File file = editor.processor.waveTableFiles[shapeCB.getSelectedItemIndex()-UserOscShapeSet];
            osc.setWaveTables(file);
            vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(1.);
            updateShapeCB();
        }
        else
        {
            float normValue = shapeCB.getSelectedItemIndex() / float(OscShapeSetNil);
            vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(normValue);
            updateShapeCB();
        }
        
        if (shapeCB.getSelectedItemIndex() == UserOscShapeSet)
        {
            // Maybe should check that the loaded table has more
            // than one waveform and set alpha accordingly
            getDial(OscShape)->setAlpha(1.f);
            getDial(OscShape)->setInterceptsMouseClicks(true, true);
        }
        else if (shapeCB.getSelectedItemIndex() > SineTriOscShapeSet &&
                 shapeCB.getSelectedItemIndex() != PulseOscShapeSet)
        {
            getDial(OscShape)->setAlpha(0.5f);
            getDial(OscShape)->setInterceptsMouseClicks(false, false);
        }
        else
        {
            getDial(OscShape)->setAlpha(1.f);
            getDial(OscShape)->setInterceptsMouseClicks(true, true);
        }
    }
}

void OscModule::mouseDown(const MouseEvent& e)
{
    updateShapeCB();
}

void OscModule::mouseEnter(const MouseEvent& e)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(e.originalComponent->getParentComponent()))
    {
        displayPitchMapping(mt);
    }
}

void OscModule::mouseExit(const MouseEvent& e)
{
    displayPitch();
}

void OscModule::updateShapeCB()
{
    shapeCB.clear(dontSendNotification);
    for (int i = 0; i < oscShapeSetNames.size()-1; ++i)
    {
        shapeCB.addItem(oscShapeSetNames[i], shapeCB.getNumItems()+1);
    }
    for (auto file : editor.processor.waveTableFiles)
    {
        shapeCB.addItem(file.getFileNameWithoutExtension(), shapeCB.getNumItems()+1);
    }
    shapeCB.addItem(oscShapeSetNames[oscShapeSetNames.size()-1], shapeCB.getNumItems()+1);
    
    RangedAudioParameter* param = vts.getParameter(ac.getName() + " ShapeSet");
    int index = param->getNormalisableRange().convertFrom0to1(param->getValue());
    if (index == UserOscShapeSet)
    {
        Oscillator& osc = static_cast<Oscillator&>(ac);
        index = editor.processor.waveTableFiles.indexOf(osc.getWaveTableFile())+UserOscShapeSet;
    }
    shapeCB.setSelectedItemIndex(index, dontSendNotification);
}

void OscModule::displayPitch()
{
    auto pitch = getDial(OscPitch)->getSlider().getValue();
    auto fine = getDial(OscFine)->getSlider().getValue()*0.01;
    pitchLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    String text = pitch+fine >= 0 ? "+" : "";
    text += String(pitch+fine, 3);
    pitchLabel.setText(text, dontSendNotification);
    
    auto freq = getDial(OscFreq)->getSlider().getValue();
    freqLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    text = freq >= 0 ? "+" : "";
    text += String(int(freq)) + " Hz";
    freqLabel.setText(text, dontSendNotification);
}

void OscModule::displayPitchMapping(MappingTarget* mt)
{
    if (!mt->isActive())
    {
        displayPitch();
        return;
    }
    auto start = mt->getModel().start;
    auto end = mt->getModel().end;
    if (mt->getParentComponent() == getDial(OscPitch))
    {
        pitchLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar())
        {
            if (mt->getSkewFactor() != 1. && start != end)
            {
                text = (start >= 0 ? "+" : "");
                text += String(start, 3) + "/";
                text += (end >= 0 ? "+" : "");
                text += String(end, 3);
            }
            else
            {
                text = String::charToString(0xb1);
                text += String(fabs(mt->getModel().end), 3);
            }
        }
        else
        {
            text = (end >= 0 ? "+" : "");
            text += String(end, 3);
        }
        pitchLabel.setText(text, dontSendNotification);
    }
    else if (mt->getParentComponent() == getDial(OscFine))
    {
        pitchLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        start *= 0.01f;
        end *= 0.01f;
        if (mt->isBipolar())
        {
            if (mt->getSkewFactor() != 1. && start != end)
            {
                text = (start >= 0 ? "+" : "");
                text += String(start, 3) + "/";
                text += (end >= 0 ? "+" : "");
                text += String(end, 3);
            }
            else
            {
                text = String::charToString(0xb1);
                text += String(fabs(mt->getModel().end), 3);
            }
        }
        else
        {
            text = (end >= 0 ? "+" : "");
            text += String(end, 3);
        }
        pitchLabel.setText(text, dontSendNotification);
    }
    else if (mt->getParentComponent() == getDial(OscFreq))
    {
        freqLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar())
        {
            if (mt->getSkewFactor() != 1. && start != end)
            {
                text = (start >= 0 ? "+" : "");
                text += String(int(start)) + "/";
                text += (end >= 0 ? "+" : "");
                text += String(int(end));
            }
            else
            {
                text = String::charToString(0xb1);
                text += String(abs(mt->getModel().end));
            }
        }
        else
        {
            text = (end >= 0 ? "+" : "");
            text += String(int(end));
        }
        freqLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

FilterModule::FilterModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ESModule(editor, vts, ac, 0.04f, 0.22f, 0.04f, 0.2f, 0.7f)
{
    outlineColour = Colours::darkgrey;
    
    double cutoff = getDial(FilterCutoff)->getSlider().getValue();
    cutoffLabel.setText(String(cutoff, 2), dontSendNotification);
    cutoffLabel.setLookAndFeel(&laf);
    cutoffLabel.setEditable(true);
    cutoffLabel.setJustificationType(Justification::centred);
    cutoffLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    cutoffLabel.addListener(this);
    addAndMakeVisible(cutoffLabel);
    
    RangedAudioParameter* set = vts.getParameter(ac.getName() + " Type");
    typeCB.addItemList(filterTypeNames, 1);
    typeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    typeCB.setLookAndFeel(&laf);
    addAndMakeVisible(typeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.getName() + " Type", typeCB));
}

FilterModule::~FilterModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void FilterModule::resized()
{
    ESModule::resized();
    
    for (int i = 1; i < ac.getParamNames().size(); ++i)
    {
        dials[i]->setBoundsRelative((relDialWidth*(i+1))+(relDialSpacing*i),
                                    relTopMargin, relDialWidth, relDialHeight);
    }
    
    cutoffLabel.setBoundsRelative(relLeftMargin+relDialWidth+0.5f*relDialSpacing,
                                  0.42f, relDialWidth-relLeftMargin, 0.16f);
    
    typeCB.setBounds(enabledToggle.getRight(), 4, getWidth()*0.3f, enabledToggle.getHeight()-4);
//    typeCB.setBoundsRelative(relLeftMargin, 0.01f, relDialWidth+relDialSpacing, 0.16f);
}

void FilterModule::sliderValueChanged(Slider* slider)
{
    if (slider == &getDial(FilterCutoff)->getSlider())
    {
        displayCutoff();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ESDial*>(mt->getParentComponent())->sliderValueChanged(slider);
        displayCutoffMapping(mt);
    }
}

void FilterModule::labelTextChanged(Label* label)
{
    if (label == &cutoffLabel)
    {
        auto value = cutoffLabel.getText().getDoubleValue();
        getDial(FilterCutoff)->getSlider().setValue(value);
    }
}

void FilterModule::mouseEnter(const MouseEvent& e)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(e.originalComponent->getParentComponent()))
    {
        displayCutoffMapping(mt);
    }
}

void FilterModule::mouseExit(const MouseEvent& e)
{
    displayCutoff();
}

void FilterModule::displayCutoff()
{
    double cutoff = getDial(FilterCutoff)->getSlider().getValue();
    cutoffLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    cutoffLabel.setText(String(cutoff, 2), dontSendNotification);
}

void FilterModule::displayCutoffMapping(MappingTarget* mt)
{
    if (!mt->isActive()) displayCutoff();
    else if (mt->getParentComponent() == getDial(FilterCutoff))
    {
        auto start = mt->getModel().start;
        auto end = mt->getModel().end;
        cutoffLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar())
        {
            if (mt->getSkewFactor() != 1. && start != end)
            {
                text = (start >= 0 ? "+" : "-");
                text += String(fabs(start), 2) + "/";
                text += (end >= 0 ? "+" : "-");
                text += String(fabs(end), 2);
            }
            else
            {
                text = String::charToString(0xb1);
                text += String(fabs(mt->getModel().end), 2);
            }
        }
        else
        {
            text = (end >= 0 ? "+" : "-");
            text += String(fabs(end), 2);
        }
        cutoffLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

EnvModule::EnvModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ESModule(editor, vts, ac, 0.03f, 0.14f, 0.06f, 0.16f, 0.84f)
{
    velocityToggle.setButtonText("Scale to velocity");
    addAndMakeVisible(velocityToggle);
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " Velocity", velocityToggle));
}

EnvModule::~EnvModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void EnvModule::resized()
{
    ESModule::resized();
    
    velocityToggle.setBoundsRelative(relLeftMargin, 0.f, 2*relDialWidth+relDialSpacing, 0.16f);
}

//==============================================================================
//==============================================================================

LFOModule::LFOModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ESModule(editor, vts, ac, 0.12f, 0.14f, 0.17f, 0.16f, 0.84f),
chooser("Select wavetable file or folder...",
        File::getSpecialLocation(File::userDocumentsDirectory))
{
    double rate = getDial(LowFreqRate)->getSlider().getValue();
    rateLabel.setText(String(rate, 2) + " Hz", dontSendNotification);
    rateLabel.setLookAndFeel(&laf);
    rateLabel.setEditable(true);
    rateLabel.setJustificationType(Justification::centred);
    rateLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    rateLabel.addListener(this);
    addAndMakeVisible(rateLabel);
    
    RangedAudioParameter* set = vts.getParameter(ac.getName() + " ShapeSet");
    shapeCB.addItemList(lfoShapeSetNames, 1);
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    shapeCB.setLookAndFeel(&laf);
    addAndMakeVisible(shapeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.getName() + " ShapeSet", shapeCB));
    
    syncToggle.setButtonText("Sync to note-on");
    addAndMakeVisible(syncToggle);
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " Sync", syncToggle));
}

LFOModule::~LFOModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void LFOModule::resized()
{
    ESModule::resized();
    
    rateLabel.setBoundsRelative(relLeftMargin-0.3*relDialSpacing, 0.f,
                                relDialWidth+0.6f*relDialSpacing, 0.16f);
    shapeCB.setBoundsRelative(relLeftMargin+relDialWidth+0.7f*relDialSpacing, 0.f,
                              relDialWidth+0.6f*relDialSpacing, 0.16f);
    syncToggle.setBoundsRelative(relLeftMargin+2*relDialWidth+1.7f*relDialSpacing, 0.f,
                                 relDialWidth+0.6f*relDialSpacing, 0.16f);
}

void LFOModule::sliderValueChanged(Slider* slider)
{
    DBG(slider->getSkewFactor());
    if (slider == &getDial(LowFreqRate)->getSlider())
    {
        displayRate();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ESDial*>(mt->getParentComponent())->sliderValueChanged(slider);
        displayRateMapping(mt);
    }
}

void LFOModule::labelTextChanged(Label* label)
{
    if (label == &rateLabel)
    {
        auto value = rateLabel.getText().getDoubleValue();
        getDial(LowFreqRate)->getSlider().setValue(value);
    }
}

void LFOModule::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &shapeCB)
    {
        if (shapeCB.getSelectedItemIndex() > SawPulseLFOShapeSet &&
            shapeCB.getSelectedItemIndex() != PulseLFOShapeSet)
        {
            getDial(LowFreqShape)->setAlpha(0.5f);
            getDial(LowFreqShape)->setInterceptsMouseClicks(false, false);
        }
        else
        {
            getDial(LowFreqShape)->setAlpha(1.f);
            getDial(LowFreqShape)->setInterceptsMouseClicks(true, true);
        }
    }
}

void LFOModule::mouseEnter(const MouseEvent& e)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(e.originalComponent->getParentComponent()))
    {
        displayRateMapping(mt);
    }
}

void LFOModule::mouseExit(const MouseEvent& e)
{
    displayRate();
}

void LFOModule::displayRate()
{
    double rate = getDial(LowFreqRate)->getSlider().getValue();
    rateLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    rateLabel.setText(String(rate, 3) + " Hz", dontSendNotification);
}

void LFOModule::displayRateMapping(MappingTarget* mt)
{
    if (!mt->isActive()) displayRate();
    else if (mt->getParentComponent() == getDial(LowFreqRate))
    {
        auto start = mt->getModel().start;
        auto end = mt->getModel().end;
        rateLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar())
        {
            if (mt->getSkewFactor() != 1. && start != end)
            {
                text = (start >= 0 ? "+" : "-");
                text += String(fabs(start), 2) + "/";
                text += (end >= 0 ? "+" : "-");
                text += String(fabs(end), 2) + " Hz";
            }
            else
            {
                text = String::charToString(0xb1);
                text += String(fabs(mt->getModel().end), 2) + " Hz";
            }
        }
        else
        {
            text = (end >= 0 ? "+" : "-");
            text += String(fabs(end), 2) + " Hz";
        }
        rateLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

OutputModule::OutputModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ESModule(editor, vts, ac, 0.088f, 0.22f, 0.088f, 0.125f, 0.75f)
{
    outlineColour = Colours::darkgrey;
    
    masterDial = std::make_unique<ESDial>(editor, "Master", "Master", false, false);
    sliderAttachments.add(new SliderAttachment(vts, "Master", masterDial->getSlider()));
    addAndMakeVisible(masterDial.get());
}

OutputModule::~OutputModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void OutputModule::resized()
{
    ESModule::resized();
    
    masterDial->setBoundsRelative(0.7f, relTopMargin, 0.2f, relDialHeight);
}

