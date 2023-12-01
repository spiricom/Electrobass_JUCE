/*
  ==============================================================================

    ElectroModules.cpp
    Created: 2 Jul 2021 3:06:27pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "ElectroModules.h"
#include "../PluginEditor.h"

//==============================================================================
//==============================================================================
ElectroComponent::ElectroComponent() :
outlineColour(Colours::transparentBlack)
{
    setInterceptsMouseClicks(false, true);
}

ElectroComponent::~ElectroComponent()
{
    
}

void ElectroComponent::paint(Graphics &g)
{
    Rectangle<int> area = getLocalBounds();
    
    g.setColour(outlineColour);
    g.drawRect(area);
}

//==============================================================================

ElectroModule::ElectroModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
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
        dials.add(new ElectroDial(editor, paramName, displayName, false, true));
        addAndMakeVisible(dials[i]);
        sliderAttachments.add(new SliderAttachment(vts, paramName, dials[i]->getSlider()));
        auto it = find(paramDestOrder.begin(), paramDestOrder.end(),dials[i]->getSlider().getName() );
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
        dials[i]->getSlider().setComponentID((String)index);
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
        if ((name == "Osc2") || (name == "Osc3"))
        {
            enabledToggle.setToggleState(false, sendNotification);
        }
        else
        {
            enabledToggle.setToggleState(ac.isEnabled(), sendNotification);
        }
        if (name != "Filter1" && name != "Filter2")
            enabledToggle.setTooltip("Send to Filters On/Off");
        addAndMakeVisible(enabledToggle);
        buttonAttachments.add(new ButtonAttachment(vts, name, enabledToggle));
        enabledToggle.setName(name);
        auto it = find(paramDestOrder.begin(), paramDestOrder.end(),name);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
        enabledToggle.setComponentID((String)index);
    }
}

ElectroModule::~ElectroModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void ElectroModule::resized()
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

void ElectroModule::sliderValueChanged(Slider* slider)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ElectroDial*>(mt->getParentComponent())->sliderValueChanged(slider);
    } else
    {
        if (ac.processor.stream)
        {
            valueChangedSliderStream(slider);
        }
            
        //SmoothedParameter* param = ac->processor->(slider->getName()
        for (int j = 0; j < ac.getParamArraySize(); j++)
        {
            String name = slider->getName();
            String _name = ac.getParameterArray(j).getFirst()->getName();
            if(name == _name)
            {
                for (int i = 0; i < ac.processor.numVoicesActive; i++)
                {
                    ac.processor.addToKnobsToSmoothArray(ac.getParameterArray(j)[i]);
                }
                break;
            }
        }
    }
}

void ElectroModule::valueChangedSliderStream(Slider* slider)
{
    float val;
    int sliderIndex =(slider->getComponentID().getIntValue());
    /*
     if (sliderIndex != "Decay" && sliderIndex != "Rate" && sliderIndex != "Attack" && sliderIndex != "Release" && sliderIndex != "Resonance")
     */
    //there's gotta be a better way, we should store a little boolean that says whether the slider is scaled with a slider and check that instead -JS
    if ((sliderIndex != 97) && (sliderIndex != 103) && (sliderIndex != 106) && (sliderIndex != 107) && (sliderIndex != 109)&& (sliderIndex != 112)&& (sliderIndex != 113)&& (sliderIndex != 115)&& (sliderIndex != 118)&& (sliderIndex != 119)&& (sliderIndex != 121)&& (sliderIndex != 124)&& (sliderIndex != 125)&& (sliderIndex != 127)&& (sliderIndex != 130)&& (sliderIndex != 135)&& (sliderIndex != 140)&& (sliderIndex != 145))
    {
        float end = slider->getRange().getEnd();
        float start = slider->getRange().getStart();
        val = (slider->getValue() / ( end - start));
        if ( start < 0.f )
        {
            val += 0.5f;
        }
    }
    else
    {
        DBG("unsym");
        val = vts.getParameter(slider->getName())->getValue();
    }
        
    if ((sliderIndex != -1) && ((sliderIndex < 1) || (sliderIndex > 12)))
    {
        ac.processor.streamValue1 = val;
        int tempId = sliderIndex + 2;
        ac.processor.streamID1 = tempId;
        DBG("Send: " + slider->getName() +
            "with id" + String(tempId) +
            " as " + String(ac.processor.streamValue1) );
        float arr[2] = {(float)tempId, val};
        editor.knobQueue.writeTo( arr, 2);
        
    }
}

void ElectroModule::buttonClicked(Button* button)
{
    if (ac.processor.stream)
    {
        ac.processor.streamValue1 = vts.getParameter(button->getName())->getValue();
        int tempId = button->getComponentID().getIntValue() + 2;
        ac.processor.streamID1 = tempId;
        //button->get
        DBG("Send: " + button->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1));
        float arr[2] = {(float)tempId, vts.getParameter(button->getName())->getValue()};
        editor.knobQueue.writeTo( arr, 2);
    }
}
            
void ElectroModule::comboBoxChanged(ComboBox *comboBox){
    if (ac.processor.stream)
    {
        ac.processor.streamValue1 = vts.getParameter(comboBox->getName())->getValue();
        int tempId = comboBox->getComponentID().getIntValue() + 2;
        ac.processor.streamID1 = tempId;
        //button->get
        DBG("Send: " + comboBox->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1)/*String(streamValue)*/);
        float arr[2] = {(float)tempId, vts.getParameter(comboBox->getName())->getValue()};
        editor.knobQueue.writeTo( arr, 2);
    }
}
            

void ElectroModule::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void ElectroModule::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
}

ElectroDial* ElectroModule::getDial (int index)
{
    return dials[index];
}

//==============================================================================
//==============================================================================

OscModule::OscModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
//float relLeftMargin, float relDialWidth,
//float relDialSpacing, float relTopMargin, float relDialHeight) :
ElectroModule(editor, vts, ac, 0.115f, 0.096f, 0.04f, 0.18f, 0.75f),
chooser(nullptr)
{
    
    
    
    outlineColour = Colours::darkgrey;
    
    // Pitch and freq dials should snap to ints
    
    getDial(OscFreq)->setRange(-2000., 2000., 1.);
    pitchDialToggle.addListener(this);
    pitchDialToggle.setTitle("Harmonic Dial");
    pitchDialToggle.setButtonText("H");
    pitchDialToggle.setToggleable(true);
    pitchDialToggle.setClickingTogglesState(true);
    pitchDialToggle.setToggleState(true, dontSendNotification);
    
    syncToggle.addListener(this);
    syncToggle.setTitle("Sync Toggle");
    syncToggle.setButtonText("Sync");
    syncToggle.setToggleable(true);
    syncToggle.setClickingTogglesState(true);
    syncToggle.setToggleState(true, dontSendNotification);
    addAndMakeVisible(syncToggle);
    
    syncType.addListener(this);
    syncType.setTitle("Sync Type");
    syncType.setButtonText("Soft");
    syncType.setToggleable(true);
    syncType.setClickingTogglesState(true);
    syncType.setToggleState(false, dontSendNotification);
    addAndMakeVisible(syncType);
    
    
    
    steppedToggle.addListener(this);
    steppedToggle.setTitle("Stepped Dial");
    steppedToggle.setButtonText("Stepped");
    steppedToggle.changeWidthToFitText();
    steppedToggle.setToggleable(true);
    steppedToggle.setClickingTogglesState(true);
    steppedToggle.setToggleState(true, dontSendNotification);
    addAndMakeVisible(steppedToggle);
    displayPitch();
    
    /* if we comment this out does it break patches (because that element is left out of VTS?)*/
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " isHarmonic", pitchDialToggle));
    pitchDialToggle.setName(ac.getName() + " isHarmonic");
    
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " isStepped", steppedToggle));
    steppedToggle.setName(ac.getName() + " isStepped");
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " isSync", syncToggle));
    syncToggle.setName(ac.getName() + " isSync");
    buttonAttachments.add(new ButtonAttachment(vts, ac.getName() + " syncType", syncType));
    syncType.setName(ac.getName() + " syncType");

    
    harmonicsLabel.setEditable(true);
    harmonicsLabel.setJustificationType(Justification::centred);
    harmonicsLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    harmonicsLabel.addListener(this);
    addAndMakeVisible(harmonicsLabel);
    
    pitchLabel.setEditable(true);
    pitchLabel.setJustificationType(Justification::centred);
    pitchLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    pitchLabel.addListener(this);
    addAndMakeVisible(pitchLabel);
    
    freqLabel.setEditable(true);
    freqLabel.setJustificationType(Justification::centred);
    freqLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    freqLabel.addListener(this);
    addAndMakeVisible(freqLabel);
    
    displayPitch();
    
    String fullName =ac.getName() + " ShapeSet";
    RangedAudioParameter* set = vts.getParameter(fullName);
    updateShapeCB();
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    shapeCB.addListener(this);
    shapeCB.addMouseListener(this, true);
    addAndMakeVisible(shapeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, fullName, shapeCB));
    shapeCB.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
    shapeCB.setComponentID((String)index);
    
    sendSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    sendSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);
    fullName = ac.getName() + " FilterSend";
    addAndMakeVisible(sendSlider);
    sliderAttachments.add(new SliderAttachment(vts,fullName , sendSlider));
    sendSlider.setName(fullName);
    auto it2 = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index2 = 0;
        // If element was found
        if (it2 != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index2 = it2 - paramDestOrder.begin();
        }
    sendSlider.setComponentID((String)index2);
    sendSlider.addListener(this);
    
    f1Label.setText("F1", dontSendNotification);
    f1Label.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(f1Label);
    
    f2Label.setText("F2", dontSendNotification);
    f2Label.setJustificationType(Justification::topRight);
    addAndMakeVisible(f2Label);
    
    s = std::make_unique<MappingSource>
    (editor, *editor.processor.getMappingSource(ac.getName()), ac.getName());
    addAndMakeVisible(s.get());
    
    getDial(OscHarm)->setVisible(true);
    getDial(OscPitch)->setVisible(true);
    getDial(OscPitch)->setRange(-24.,24., 1.);
    getDial(OscHarm)->setRange(-15.,15., 1.);
    f1Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
    f2Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
    sendSlider.setEnabled(enabledToggle.getToggleState());
    sendSlider.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
}

OscModule::~OscModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void OscModule::resized()
{
    ElectroModule::resized();
    
    s->setBounds(4, 4, (int)(getWidth()*0.1f), enabledToggle.getHeight()-8);
   
    harmonicsLabel.setBoundsRelative(relLeftMargin,
                                     0.02f, 0.1f, 0.16f);
    
    pitchDialToggle.setBoundsRelative(0.0f, 0.5f, 0.05f, 0.15f);
    syncToggle.setBoundsRelative(0.0f, 0.6f, 0.05f, 0.15f);
    syncType.setBoundsRelative(0.0f, 0.75f, 0.05f, 0.15f);
    steppedToggle.setBoundsRelative(0.0f, 0.2f, 0.1f, 0.15f);

    
    
    pitchLabel.setBoundsRelative(relLeftMargin+(1.0f*(relDialWidth+relDialSpacing)),
                                 0.02f, .1f, 0.16f);
    
    freqLabel.setBoundsRelative(relLeftMargin+(3.0f*(relDialWidth+relDialSpacing)),
                                 0.02f, .1f, 0.16f);
    
    shapeCB.setBoundsRelative(relLeftMargin+(4.0f*(relDialWidth+relDialSpacing)), 0.02f,
                              .16f, 0.16f);
    
    sendSlider.setBoundsRelative(0.96f, 0.f, 0.04f, 1.0f);
    
    enabledToggle.setBoundsRelative(0.917f, 0.5f, 0.04f, 0.15f);
    smoothingToggle.setBoundsRelative(0.0f, 0.5f, 0.5f, 0.15f);
    f1Label.setBoundsRelative(0.9f, 0.05f, 0.06f, 0.15f);
    f2Label.setBoundsRelative(0.9f, 0.80f, 0.06f, 0.15f);
    //draw harmonics knob first
    dials[dials.size()-1]->setBoundsRelative(relLeftMargin , relTopMargin,
                               relDialWidth, relDialHeight);
    //then the other ones
    for (int i = 0; i < ac.getParamNames().size() - 1; ++i)
    {
        dials[i]->setBoundsRelative(relLeftMargin + (relDialWidth+relDialSpacing)*(i+1), relTopMargin,
                                    relDialWidth, relDialHeight);
    }
    
}

void OscModule::sliderValueChanged(Slider* slider)
{
    
    
    if (slider == &getDial(OscPitch)->getSlider() ||
        slider == &getDial(OscFine)->getSlider() ||
        slider == &getDial(OscFreq)->getSlider() || slider == &getDial(OscHarm)->getSlider())
    {
        displayPitch();
    }
//    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
//    {
//        dynamic_cast<ElectroDial*>(mt->getParentComponent())->sliderValueChanged(slider);
//        displayPitchMapping(mt);
//    }
    
    ElectroModule::sliderValueChanged(slider);
}
    //ElectroModule::sliderValueChanged(slider);
    //ElectroModule.sliderValueChanged(slider);


void OscModule::buttonClicked(Button* button)
{
    if (button == &enabledToggle)
    {
        f1Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
        f2Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
        sendSlider.setEnabled(enabledToggle.getToggleState());
        sendSlider.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
        
    }
    else if (button == &steppedToggle)
    {
        
        if (!steppedToggle.getToggleState())
        {
            getDial(OscPitch)->setRange(-24, 24., steppedToggle.getToggleState() ? 1 : 0.01 );
            getDial(OscHarm)->setRange(-15, 15., steppedToggle.getToggleState() ? 1 : 0.01 );
            steppedToggle.setButtonText("Smoothed");
        }
        else
        {
            getDial(OscHarm)->setRange(-15, 15., steppedToggle.getToggleState() ? 1 : 0.01 );
            getDial(OscPitch)->setRange(-24, 24., steppedToggle.getToggleState() ? 1 : 0.01 );
            steppedToggle.setButtonText("Stepped");
        }
        displayPitch();
    }
    else if (button == &syncToggle)
    {
        if(syncToggle.getToggleState())
        {
            syncType.setAlpha(1.0f);
            syncType.setInterceptsMouseClicks(true,true);
        }
        else
        {
            syncType.setAlpha(0.5f);
            syncType.setInterceptsMouseClicks(false,false);
        }
    } else if (button == &syncType)
    {
        if(syncType.getToggleState())
            syncType.setButtonText("Hard");
        else
            syncType.setButtonText("Soft");
    }
    ElectroModule::buttonClicked(button);
}
void OscModule::labelTextChanged(Label* label)
{
    if (ac.processor.stream)
    {
        labelTextChange = true;
    }
    if (label == &pitchLabel)
    {
        auto value = pitchLabel.getText().getDoubleValue();
        int i = (int)value;
        double f = value-i;
        //getDial(OscPitch)->getSlider().setValue(i, sendNotificationAsync);
        getDial(OscFine)->getSlider().setValue(f*100.0f, sendNotificationSync);
    }
    else if (label == &freqLabel)
    {
        auto value = freqLabel.getText().getDoubleValue();
        getDial(OscFreq)->getSlider().setValue(value, sendNotificationSync);
    }
    else if (label == &harmonicsLabel)
    {
        auto value = harmonicsLabel.getText().getDoubleValue();
        getDial(OscHarm)->getSlider().setValue(value, sendNotificationSync);
    }
    
}

void OscModule::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &shapeCB)
    {
        // Select file... option
#if 0
        if (shapeCB.getSelectedItemIndex() == shapeCB.getNumItems()-1)
        {
            if (chooser != nullptr)
                delete chooser;
            chooser = new FileChooser ("Pick a wav table", editor.processor.getLastFile());
            chooser->launchAsync (FileBrowserComponent::openMode |
                                 FileBrowserComponent::canSelectFiles,
                                 [this] (const FileChooser& chooser)
                                 {
                editor.processor.setLastFile(chooser);
                String path = chooser.getResult().getFullPathName();
                Oscillator& osc = static_cast<Oscillator&>(ac);
                editor.processor.setLastFile(chooser);
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
#endif
        // Selected a loaded file
#if 0
        else if (shapeCB.getSelectedItemIndex() >= UserOscShapeSet)
        {
            Oscillator& osc = static_cast<Oscillator&>(ac);
            File file = editor.processor.waveTableFiles[shapeCB.getSelectedItemIndex()-UserOscShapeSet];
            osc.setWaveTables(file);
            vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(1.);
            updateShapeCB();
        }
        
        // Selected built ins
        else
#endif
        {
            //float normValue = shapeCB.getSelectedItemIndex() / float(OscShapeSetNil);
            //vts.getParameter(ac.getName() + " ShapeSet")->setValueNotifyingHost(normValue);
            //updateShapeCB();
        }
#if 0
        if (shapeCB.getSelectedItemIndex() >= UserOscShapeSet)
        {
            // Maybe should check that the loaded table has more
            // than one waveform and set alpha accordingly
            getDial(OscShape)->setAlpha(1.f);
            getDial(OscShape)->setInterceptsMouseClicks(true, true);
            syncToggle.setAlpha(0.5);
            syncToggle.setInterceptsMouseClicks(false, false);
            syncType.setAlpha(0.5f);
            syncType.setInterceptsMouseClicks(false,false);
        }
#endif
        //else if
        if
            (shapeCB.getSelectedItemIndex() > SineTriOscShapeSet &&
                 shapeCB.getSelectedItemIndex() != PulseOscShapeSet && shapeCB.getSelectedItemIndex() != TriOscShapeSet)
        {
            getDial(OscShape)->setAlpha(0.5f);
            getDial(OscShape)->setInterceptsMouseClicks(false, false);
            
        }
        else
        {
            getDial(OscShape)->setAlpha(1.f);
            getDial(OscShape)->setInterceptsMouseClicks(true, true);
        }
        if (shapeCB.getSelectedItemIndex() != SineOscShapeSet)
        {
            syncToggle.setAlpha(1.0);
            syncToggle.setInterceptsMouseClicks(true, true);
            syncType.setAlpha(1.0);
            syncType.setInterceptsMouseClicks(true,true);
        }
    }
    if (ac.processor.stream)
    {
        ac.processor.streamValue1 = (float)comboBox->getSelectedItemIndex()/ ((float) (oscShapeSetNames.size()-1));
        int tempId = comboBox->getComponentID().getIntValue() + 2;
        ac.processor.streamID1 = tempId;
        DBG("Send: " + comboBox->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1)/*String(streamValue)*/);
        float arr[2] = {(float)tempId, (float)comboBox->getSelectedItemIndex()/ ((float) (oscShapeSetNames.size()-1))};
        editor.knobQueue.writeTo( arr, 2);
        
        
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
    //if (index == UserOscShapeSet)
    {
        //Oscillator& osc = static_cast<Oscillator&>(ac);
       // index = editor.processor.waveTableFiles.indexOf(osc.getWaveTableFile())+UserOscShapeSet;
    }
    shapeCB.setSelectedItemIndex(index, dontSendNotification);
}
//MAKE CHANGE HERE
void OscModule::displayPitch()
{
    
   
    auto harm = getDial(OscHarm)->getSlider().getValue();
    harmonicsLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    String t = harm >= 0 ? String(abs(harm + 1),2) : String("1/" + String(abs(harm-1),2));
   
        harmonicsLabel.setText(t, dontSendNotification);
    
    auto pitch = getDial(OscPitch)->getSlider().getValue();
    auto fine = getDial(OscFine)->getSlider().getValue()*0.01;
    pitchLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    String text = pitch+fine >= 0 ? "+" : "";
    text += String(pitch+fine, 2);
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

NoiseModule::NoiseModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.01f, 0.2f, 0.02f, 0.18f, 0.8f)
{
    outlineColour = Colours::darkgrey;
    
    sendSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    sendSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);
    addAndMakeVisible(sendSlider);
    String fullName =ac.getName() + " FilterSend";
    sliderAttachments.add(new SliderAttachment(vts, fullName, sendSlider));
    sendSlider.addListener(this);
    sendSlider.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
    sendSlider.setComponentID((String)index);
    
    f1Label.setText("F1", dontSendNotification);
    f1Label.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(f1Label);
    
    f2Label.setText("F2", dontSendNotification);
    f2Label.setJustificationType(Justification::topRight);
    addAndMakeVisible(f2Label);
    
    s = std::make_unique<MappingSource>
    (editor, *editor.processor.getMappingSource(ac.getName()), ac.getName());
    addAndMakeVisible(s.get());
}

NoiseModule::~NoiseModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void NoiseModule::resized()
{
    ElectroModule::resized();
    
    s->setBounds(4, 4, (int)(getWidth()*0.2f), enabledToggle.getHeight()-8);
    s->toFront(false);
    
    sendSlider.setBoundsRelative(0.92f, 0.f, 0.08f, 1.0f);
    
    enabledToggle.setBoundsRelative(0.86f, 0.4f, 0.1f, 0.18f);

    
    
    f1Label.setBoundsRelative(0.81f, 0.05f, 0.12f, 0.17f);
    f2Label.setBoundsRelative(0.81f, 0.78f, 0.12f, 0.17f);
    
}

void NoiseModule::buttonClicked(Button* button)
{
    if (button == &enabledToggle)
    {
        f1Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
        f2Label.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
        sendSlider.setEnabled(enabledToggle.getToggleState());
        sendSlider.setAlpha(enabledToggle.getToggleState() ? 1. : 0.5);
    }
    ElectroModule::buttonClicked(button);
}

//==============================================================================
//==============================================================================

FilterModule::FilterModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.04f, 0.215f, 0.05f, 0.18f, 0.8f) //0.05f, 0.132f, 0.05f, 0.18f, 0.8f),
{
    outlineColour = Colours::darkgrey;
    String fullName =ac.getName() + " Type";
    RangedAudioParameter* set = vts.getParameter(fullName);
    typeCB.addItemList(filterTypeNames, 1);
    typeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    addAndMakeVisible(typeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, fullName, typeCB));
    typeCB.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
    typeCB.setComponentID((String)index);
    typeCB.addMouseListener(this, true);
    typeCB.addListener(this);
}

FilterModule::~FilterModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void FilterModule::resized()
{
    ElectroModule::resized();
    
    for (int i = 0; i < ac.getParamNames().size(); ++i)
    {
        dials[i]->setBoundsRelative((relDialWidth*(i))+(relDialSpacing * 0.6f*(i+1)),
                                    relTopMargin, relDialWidth, relDialHeight);
    }
    

    typeCB.setBounds(enabledToggle.getRight(), 4, (int)(getWidth()*0.3f), enabledToggle.getHeight()-4);
//    typeCB.setBoundsRelative(relLeftMargin, 0.01f, relDialWidth+relDialSpacing, 0.16f);
}

void FilterModule::sliderValueChanged(Slider* slider)
{
    if (slider == &getDial(FilterCutoff)->getSlider())
    {
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        dynamic_cast<ElectroDial*>(mt->getParentComponent())->sliderValueChanged(slider);
    }
    ElectroModule::sliderValueChanged(slider);
}


void FilterModule::mouseEnter(const MouseEvent& e)
{
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(e.originalComponent->getParentComponent()))
    {
    }
}

void FilterModule::mouseExit(const MouseEvent& e)
{
}



//==============================================================================
//==============================================================================

EnvModule::EnvModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.03f, 0.14f, 0.06f, 0.16f, 0.84f)
{
    velocityToggle.setButtonText("Scale to velocity");
    addAndMakeVisible(velocityToggle);
    String fullName =ac.getName() + " Velocity";
    buttonAttachments.add(new ButtonAttachment(vts, fullName, velocityToggle));
    velocityToggle.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
    velocityToggle.setComponentID((String)index);
    velocityToggle.addListener(this);
}

EnvModule::~EnvModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void EnvModule::resized()
{
    ElectroModule::resized();
    
    velocityToggle.setBoundsRelative(relLeftMargin, 0.f, 2*relDialWidth+relDialSpacing, 0.16f);
}



//==============================================================================
//==============================================================================

LFOModule::LFOModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.12f, 0.14f, 0.17f, 0.16f, 0.84f),
chooser("Select wavetable file or folder...",
        File::getSpecialLocation(File::userDocumentsDirectory))
{
    double rate = getDial(LowFreqRate)->getSlider().getValue();
    rateLabel.setText(String(rate, 2) + " Hz", dontSendNotification);
    rateLabel.setEditable(true);
    rateLabel.setJustificationType(Justification::centred);
    rateLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    rateLabel.addListener(this);
    addAndMakeVisible(rateLabel);
    String fullName = ac.getName() + " ShapeSet";
    RangedAudioParameter* set = vts.getParameter(fullName);
    shapeCB.addItemList(lfoShapeSetNames, 1);
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    addAndMakeVisible(shapeCB);

    comboBoxAttachments.add(new ComboBoxAttachment(vts, fullName, shapeCB));
    shapeCB.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index = 0;
        // If element was found
        if (it != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index = it - paramDestOrder.begin();
        }
    shapeCB.setComponentID((String)index);
    shapeCB.addListener(this);
    shapeCB.addMouseListener(this, true);
    
    syncToggle.setButtonText("Sync to note-on");
    syncToggle.addListener(this);
    addAndMakeVisible(syncToggle);
    fullName = ac.getName() + " Sync";
    buttonAttachments.add(new ButtonAttachment(vts, fullName, syncToggle));
    syncToggle.setName(fullName);
    auto it2 = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
        int index2 = 0;
        // If element was found
        if (it2 != paramDestOrder.end())
        {
          
          // calculating the index
          // of K
          index2 = it2 - paramDestOrder.begin();
        }
    syncToggle.setComponentID((String)index2);
    
}

LFOModule::~LFOModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void LFOModule::resized()
{
    ElectroModule::resized();
    
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
        dynamic_cast<ElectroDial*>(mt->getParentComponent())->sliderValueChanged(slider);
        displayRateMapping(mt);
    }
    ElectroModule::sliderValueChanged(slider);
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
    if (ac.processor.stream)
    {
        ac.processor.streamValue1 = (float)comboBox->getSelectedItemIndex()/ ((float) (lfoShapeSetNames.size()-1));
        
        int tempId = comboBox->getComponentID().getIntValue() + 2;
        ac.processor.streamID1 = tempId;
        //button->get
        DBG("Send: " + comboBox->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1)/*String(streamValue)*/);
        ac.processor.addToMidiBuffer(tempId, (float)comboBox->getSelectedItemIndex()/ ((float) (lfoShapeSetNames.size()-1)));
        float arr[2] = {(float)tempId, (float)comboBox->getSelectedItemIndex()/ ((float) (lfoShapeSetNames.size()-1))};
        editor.knobQueue.writeTo( arr, 2);
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

OutputModule::OutputModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.07f, 0.22f, 0.07f, 0.07f, 0.78f)
{
    outlineColour = Colours::darkgrey;
    //masterDial = std::make_unique<ElectroDial>(editor, "Master", "Master", false, false);
    fxPostButton.setRadioGroupId(1001);
    fxPostButton.setClickingTogglesState(true);
    fxPreButton.setRadioGroupId(1001);
    fxPreButton.setClickingTogglesState(true);
    
    //addAndMakeVisible(fxPreButton);
    addAndMakeVisible(fxPreButton);
    addAndMakeVisible(fxPostButton);
    fxPreButton.addListener(this);
    fxPostButton.addListener(this);
    fxPreButton.setName("FX Order");
    fxPostButton.setName("FX Order");
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),"FX Order");
    int index = 0;
    // If element was found
    if (it != paramDestOrder.end())
    {
      
      // calculating the index
      // of K
      index = it - paramDestOrder.begin();
    }
    fxPreButton.setComponentID((String)index);
    fxPostButton.setComponentID((String)index);
    getDial(OutputAmp)->getTargets()[2]->setRemovable(false);
   // updateFXOrder(1.0f);
    updateFXOrder(vts.getRawParameterValue("FX Order")->load());
}

void OutputModule::updateFXOrder(float val)
{
    if (val >= 1.f)
    {
    
        fxPreButton.setToggleState(false,dontSendNotification);
        fxPostButton.setToggleState(true,dontSendNotification);
    } else
    {
        fxPreButton.setToggleState(true,dontSendNotification);
        fxPostButton.setToggleState(false,dontSendNotification);
        
    }
}


void OutputModule::updateFXOrder(Button *button)
{
    if(button == &fxPreButton)
    {
        vts.getParameter("FX Order")->setValueNotifyingHost(0.f);
        
    }
    else if(button == &fxPostButton)
    {
        vts.getParameter("FX Order")->setValueNotifyingHost(1.f);
    }
    if (ac.processor.stream)
    {
        ac.processor.streamValue1 = vts.getParameter("FX Order")->getValue();
        int tempId = button->getComponentID().getIntValue() + 2;
        ac.processor.streamID1 = tempId;
        //button->get
        DBG("Send:  FXORder with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1));

        float arr[2] = {(float)tempId, vts.getParameter("FX Order")->getValue()};
        editor.knobQueue.writeTo( arr, 2);
    }
}
void OutputModule::buttonClicked(Button* button)
{
    updateFXOrder(button);
}


OutputModule::~OutputModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}


void OutputModule::resized()
{
    ElectroModule::resized();
    getDial(OutputTone)->setBoundsRelative(0.7f, relTopMargin, 0.17f, relDialHeight);
    getDial(OutputAmp)->setBoundsRelative(0.3f, relTopMargin, 0.17f, relDialHeight);
    
    fxPostButton.setBoundsRelative(0.5f, relTopMargin + 0.2f, 0.17f, relDialHeight - 0.4f);
    fxPreButton.setBoundsRelative(0.1f,relTopMargin + 0.2f, 0.17f, relDialHeight- 0.4f);
   // meters.setBoundsRelative(.2f, relTopMargin - 0.08f, 0.2f, relDialHeight*1.6f);
    //meters.setAlwaysOnTop(true);
    //setVerticalRotatedWithBounds(meters, true, Rectangle<int>(masterDial->getX(), masterDial->getY(), masterDial->getHeight(), masterDial->getWidth()));
}

//==============================================================================

FXModule::FXModule(ElectroAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ElectroModule(editor, vts, ac, 0.03f, 0.115f, 0.025f, 0.17f, 0.80f)
{
    RangedAudioParameter* set = vts.getParameter(ac.getName() + " FXType");
    fxCB.addItemList(FXTypeNames, 1);
    fxCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()), dontSendNotification);
    addAndMakeVisible(fxCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.getName() + " FXType", fxCB));
    String fullName = ac.getName() + " FXType";
    fxCB.setName(fullName);
    auto it = find(paramDestOrder.begin(), paramDestOrder.end(),fullName);
    int index = 0;
    // If element was found
    if (it != paramDestOrder.end())
    {
      
      // calculating the index
      // of K
      index = it - paramDestOrder.begin();
    }
    fxCB.setComponentID((String)index);
    fxCB.addListener(this);
    fxCB.addMouseListener(this, true);
    setNamesAndDefaults((FXType)fxCB.getSelectedItemIndex());
}

FXModule::~FXModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void FXModule::resized()
{
    ElectroModule::resized();
    fxCB.setBoundsRelative(0.01f, 0.02f,
                              relDialWidth+0.6f*relDialSpacing, 0.16f);
    
}

void FXModule::paint(Graphics &g)
{
    ElectroModule::paint(g);
    
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds,2);
}


void FXModule::comboBoxChanged(ComboBox *comboBox)
{
   if (comboBox == &fxCB)
    {
        if (ac.processor.stream)
        {
            ac.processor.streamValue1 = (float)fxCB.getSelectedItemIndex()/ ((float) (FXTypeNames.size()-1));
            int tempId = comboBox->getComponentID().getIntValue() + 2;
            ac.processor.streamID1 = tempId;
            //button->get
            DBG("Send: " + comboBox->getName() + " with ID"  + String(tempId) + " and value " + String(ac.processor.streamValue1)/*String(streamValue)*/);
            ac.processor.addToMidiBuffer(tempId, (float)fxCB.getSelectedItemIndex()/ ((float) (FXTypeNames.size()-1)));
            float arr[2] = {(float)tempId, (float)fxCB.getSelectedItemIndex()/ ((float) (FXTypeNames.size()-1))};
            editor.knobQueue.writeTo( arr, 2);
        }
        setNamesAndDefaults((FXType)fxCB.getSelectedItemIndex());
        if(fxCB.isVisible())
        {
            for (int i = 0; i < FXParam::Mix; i++)
            {
                float g =  FXParamDefaults[(FXType)fxCB.getSelectedItemIndex()][i];
                getDial(i)->setValueNotif(g, sendNotification);
                if (ac.processor.stream)
                {
                  
                    float arr[2] = {(float)(comboBox->getComponentID().getIntValue() + 3 + i), g};
                    editor.knobQueue.writeTo( arr, 2);
                }
            }
        }
    }
    
//    ElectroModule::comboBoxChanged(comboBox);
}

void FXModule::setNamesAndDefaults(FXType effect)
{
    //if (ac.processor.stream)
        //while(ac.processor.streamSend);
    for (int i = 0; i < FXParam::Mix; i++)
    {
        getDial(i)->setText(FXParamNames[effect][i], dontSendNotification);
       
        
        if (FXParamNames[effect][i].isEmpty())
        {
            getDial(i)->setAlpha(0.5);
            getDial(i)->setEnabled(false);
        } else
        {
            getDial(i)->setAlpha(1);
            getDial(i)->setEnabled(true);
        }
        
        
    }
//    if (FXType > Wavefolder)
//    {
//        getDial(Param1)->getSlider()->setRange
//        getDial(Param2)->getSlider()->setNormalisableRange();
//    }
}


void FilterModule::comboBoxChanged(ComboBox *comboBox)
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
        
        float arr[2] = {(float)tempId, (float)comboBox->getSelectedItemIndex()/ ((float) (filterTypeNames.size()-1))};
        editor.knobQueue.writeTo( arr, 2);
    }
};
