/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
//==============================================================================

ESAudioProcessorEditor::ESAudioProcessorEditor (ESAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioProcessorEditor (&p),
processor (p),
vts(vts),
keyboard(p.keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain.get())),
chooser("Select a .wav file to load...", {}, "*.wav")
{
    //    panel = Drawable::createFromImageData(BinaryData::panel_svg, BinaryData::panel_svgSize);
    
    setWantsKeyboardFocus(true);
    
    getTopLevelComponent()->addKeyListener(this);
        
    Typeface::Ptr tp = Typeface::createSystemTypefaceFor(BinaryData::EuphemiaCAS_ttf,
                                                         BinaryData::EuphemiaCAS_ttfSize);
    euphemia = Font(tp);
    //    euphemia.setItalic(true);
    
    masterDial = std::make_unique<ESDial>("Master");
    masterDial->addListener(this);
    addAndMakeVisible(masterDial.get());
    sliderAttachments.add(new SliderAttachment(vts, "Master", masterDial->getSlider()));
    
    mappingTargets.add(new MappingTarget("Master", processor.masterVolume));
    mappingTargets[0]->addListener(this);
    addAndMakeVisible(mappingTargets[0]);
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
        ccDials.add(new ESDial("CC" + String(i+1)));
        ccDials[i]->addListener(this);
        addAndMakeVisible(ccDials[i]);
        sliderAttachments.add(new SliderAttachment(vts, "CC" + String(i+1), ccDials[i]->getSlider()));

        mappingSources.add(new MappingSource("CC" + String(i+1),
                                             processor.ccParams[i]->getValuePointer()));
        mappingSources[i]->addListener(this);
        addAndMakeVisible(mappingSources[i]);
    }
    currentMappingSource = nullptr;
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        channelSelection.add(new TextButton(String(i + 1)));
        channelSelection[i]->setLookAndFeel(&laf);
        channelSelection[i]->setConnectedEdges(Button::ConnectedOnLeft & Button::ConnectedOnRight);
        channelSelection[i]->setRadioGroupId(1);
        channelSelection[i]->setClickingTogglesState(true);
        channelSelection[i]->addListener(this);
        addAndMakeVisible(channelSelection[i]);
        
        pitchBendSliders.add(new Slider());
        pitchBendSliders[i]->setLookAndFeel(&laf);
        pitchBendSliders[i]->setRange(-24., 24.);
        pitchBendSliders[i]->addListener(this);
        addAndMakeVisible(pitchBendSliders[i]);
        
        sliderAttachments.add(new SliderAttachment(vts, "PitchBendCh" + String(i+1),
                                                   *pitchBendSliders[i]));
    }
    channelSelection[0]->setButtonText("1 (Global)");
    channelSelection[1]->setToggleState(true, sendNotification);
    
    keyboard.setAvailableRange(21, 108);
    keyboard.setOctaveForMiddleC(4);
    addAndMakeVisible(&keyboard);
    
    //==============================================================================
    
    modules.add(new ESModule(vts, *processor.sposc[0]));
    modules.add(new ESModule(vts, *processor.lp[0]));
    modules.add(new ESModule(vts, *processor.env[0]));
    modules.add(new ESModule(vts, *processor.env[1]));
    
    addAndMakeVisible(modules[0]);
    addAndMakeVisible(modules[1]);
    addAndMakeVisible(modules[2]);
    addAndMakeVisible(modules[3]);
    
    //==============================================================================
    
    setSize(EDITOR_WIDTH * processor.editorScale, EDITOR_HEIGHT * processor.editorScale);
    
    constrain->setFixedAspectRatio(EDITOR_WIDTH / EDITOR_HEIGHT);
    
    addAndMakeVisible(*resizer);
    resizer->setAlwaysOnTop(true);
    
    versionLabel.setJustificationType(Justification::topRight);
    versionLabel.setBorderSize(BorderSize<int>(2));
    versionLabel.setText("v" + String(ProjectInfo::versionString), dontSendNotification);
    versionLabel.setColour(Label::ColourIds::textColourId, Colours::lightgrey);
    addAndMakeVisible(versionLabel);
    
    startTimerHz(30);
}

ESAudioProcessorEditor::~ESAudioProcessorEditor()
{
    masterDial->setLookAndFeel(nullptr);
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
        ccDials[i]->setLookAndFeel(nullptr);
    }
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        channelSelection[i]->setLookAndFeel(nullptr);
        pitchBendSliders[i]->setLookAndFeel(nullptr);
    }
}

//==============================================================================
void ESAudioProcessorEditor::paint (Graphics& g)
{
    float s = processor.editorScale;
    
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(0,0), Colour(10, 10, 10), juce::Point<float>(0, getHeight()), false));
    
    g.fillRect(0, 0, getWidth(), getHeight());
    
//    Rectangle<float> panelArea = getLocalBounds().toFloat();
//    panelArea.reduce(getWidth()*0.025f, getHeight()*0.01f);
//    panelArea.removeFromBottom(getHeight()*0.03f);
//    panel->drawWithin(g, panelArea, RectanglePlacement::centred, 1.0f);
    
    g.fillRect(getWidth() * 0.25f, getHeight() * 0.25f, getWidth() * 0.6f, getHeight() * 0.5f);
    g.fillRect(getWidth() * 0.25f, getHeight() * 0.75f, getWidth() * 0.2f, getHeight() * 0.15f);
    
//    if (currentMappingSource != nullptr)
//    {
//        g.setColour(Colours::gold);
//        g.drawLine(getMouseXYRelative().getX(), getMouseXYRelative().getY(),
//                   currentMappingSource->getX() + 10*s, currentMappingSource->getY() + 10*s);
//    }
}

void ESAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();
    
    float s = width / EDITOR_WIDTH;
    processor.editorScale = s;
    
    const float knobSize = 57.0f*s;
    const float labelWidth = 130.0f*s;
    const float labelHeight = 20.0f*s;
    
    const float masterSize = knobSize * 1.5f;
    masterDial->setBounds(0, 0, width, height);
    masterDial->setSliderBounds(550*s, 20*s, masterSize, masterSize*1.35f);
    masterDial->setLabelBounds(masterDial->getSlider().getX() + masterSize*0.5f - labelWidth*0.5f,
                               masterDial->getSlider().getY() + masterSize*1.325f,
                               labelWidth, labelHeight);
    
    mappingTargets[0]->setBounds(550*s + masterSize, 20*s, 20*s, 20*s);
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
        ccDials[i]->setBounds(0, 0, width, height);
        ccDials[i]->setSliderBounds(450*s + 90*s*i, 460*s, knobSize, knobSize * 1.35f);
        ccDials[i]->setLabelBounds(ccDials[i]->getSlider().getX() + knobSize*0.5f - labelWidth*0.5f,
                                   ccDials[i]->getSlider().getY() + knobSize*1.325f,
                                   labelWidth, labelHeight);
        
        mappingSources[i]->setBounds(450*s + 90*s*i + knobSize, 460*s, 20*s, 20*s);
    }
    
    modules[0]->setBounds(0, 20*s, 500*s, 110*s);
    modules[1]->setBounds(0, 130*s, 500*s, 110*s);
    modules[2]->setBounds(0, 240*s, 500*s, 110*s);
    modules[3]->setBounds(0, 350*s, 500*s, 110*s);
    
//    
//    for (auto label : stDialLabels)
//        label->setFont(euphemia.withHeight(height * 0.027f));
    
    resizeChannelSelection();
    
    int w = width - channelSelection[15]->getRight();
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBendSliders[i]->setBounds(channelSelection[15]->getRight(), height * 0.81f, w, height * 0.06f);
    }
    
    keyboard.setBounds(0, height * 0.87f, width, height * 0.13f);
    keyboard.setKeyWidth(width / 52.0f);

    versionLabel.setBounds(width*0.95, 0, width * 0.05f, height * 0.03f);
    versionLabel.setFont(euphemia.withHeight(height * 0.025f));
    
    float r = EDITOR_WIDTH / EDITOR_HEIGHT;
    constrain->setSizeLimits(200, 200/r, 800*r, 800);
    resizer->setBounds(getWidth()-12, getHeight()-12, 12, 12);
}

void ESAudioProcessorEditor::resizeChannelSelection()
{
    int width = getWidth();
    int height = getHeight();
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        float yf = 0.822f;
        if (channelSelection[i]->getToggleState()) yf -= 0.005f;
        float offset = width * 0.07f;
        float w = width * 0.04f;
        if (i == 0)
        {
            w += offset;
            offset = 0.0f;
        }
        Rectangle<float> bounds ((width * 0.04f - 1.f) * i + offset, height * yf, w, height * 0.06f);
        channelSelection[i]->setBounds(Rectangle<int>::leftTopRightBottom ((bounds.getX()),
                                                                           (bounds.getY()),
                                                                           (bounds.getRight()),
                                                                           (bounds.getBottom())));
    }
}

void ESAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == nullptr) return;
}

void ESAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == nullptr) return;
    
    resizeChannelSelection();
    
    if (TextButton* tb = dynamic_cast<TextButton*>(button))
    {
        if (channelSelection.contains(tb))
        {
            int channel = channelSelection.indexOf(tb) + 1;
            keyboard.setMidiChannel(channel);
            keyboard.setAlpha(channel > 1 ? 1.0f : 0.5f);
            keyboard.setInterceptsMouseClicks(channel > 1, channel > 1);
            
            for (int i = 0; i < NUM_CHANNELS; ++i)
            {
                pitchBendSliders[i]->setVisible(false);
                if (i+1 == channel) pitchBendSliders[i]->setVisible(true);
            }
        }
    }
    
    if (MappingSource* ms = dynamic_cast<MappingSource*>(button))
    {
        setMouseCursor(MouseCursor::CrosshairCursor);
        for (auto c : getAllChildren())
        {
            c->setMouseCursor(MouseCursor::CrosshairCursor);
        }
        if (currentMappingSource == ms) currentMappingSource = nullptr;
        else currentMappingSource = ms;
    }
    
    if (MappingTarget* mt = dynamic_cast<MappingTarget*>(button))
    {
        setMouseCursor(MouseCursor::NormalCursor);
        for (auto c : getAllChildren())
        {
            c->setMouseCursor(MouseCursor::NormalCursor);
        }
        if (currentMappingSource != nullptr)
        {
            mt->createMapping(currentMappingSource, true);
            currentMappingSource = nullptr;
        }
        mt->viewMappings();
    }
    
}

void ESAudioProcessorEditor::buttonStateChanged(Button* button)
{
    if (button == nullptr) return;
}

void ESAudioProcessorEditor::mouseDown (const MouseEvent &event)
{
    if (event.mods.isRightButtonDown() ||
        event.mods.isCtrlDown() ||
        event.mods.isCommandDown() ||
        event.mods.isAltDown() ||
        event.mods.isShiftDown())
    {
        setMouseCursor(MouseCursor::NormalCursor);
        for (auto c : getAllChildren())
        {
            c->setMouseCursor(MouseCursor::NormalCursor);
        }
        currentMappingSource = nullptr;
    }
}

bool ESAudioProcessorEditor::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    if (key.isKeyCode('0'))
    {
        channelSelection[9]->setToggleState(true, sendNotification);
        return true;
    }
    else if (key.isKeyCode('1'))
    {
        if (channelSelection[0]->getToggleState())
            channelSelection[10]->setToggleState(true, sendNotification);
        else channelSelection[0]->setToggleState(true, sendNotification);
        return true;
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            int d = 0;
            if (KeyPress::isKeyCurrentlyDown('1')) d = 10;
            if (key.isKeyCode('2' + i))
            {
                if (i + d >= 15) return false;
                channelSelection[i + d + 1]->setToggleState(true, sendNotification);
                return true;
            }
        }
    }
    
    return false;
}

void ESAudioProcessorEditor::timerCallback()
{

}

void ESAudioProcessorEditor::loadWav()
{
    chooser.launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                         [this] (const FileChooser& chooser)
                         {
        
        auto result = chooser.getResult();
        
        auto* reader = processor.formatManager.createReaderFor (result);
        
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource
                                                                      (reader, true));
            
            AudioBuffer<float> buffer = AudioBuffer<float>(reader->numChannels, int(reader->lengthInSamples));
            
            reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);
            
//            processor.tableSynth.setWavetable(buffer)
            
            processor.readerSource.reset(newSource.release());
            
            processor.wavetablePaths.set(0, result.getFullPathName());
        }
    });
}

void ESAudioProcessorEditor::getAllChildren(Component* component, Array<Component*> &children)
{
    for (auto c : component->getChildren())
    {
        children.add(c);
        getAllChildren(c, children);
    }
}

Array<Component*> ESAudioProcessorEditor::getAllChildren()
{
    Array<Component*> children;
    getAllChildren(this, children);
    return children;
}

//==============================================================================
//==============================================================================

ESModule::ESModule(AudioProcessorValueTreeState& vts, AudioComponent& ac) :
vts(vts),
ac(ac)
{
    setInterceptsMouseClicks(false, true);
    
    for (int i = 0; i < ac.paramNames.size(); i++)
    {
        dials.add(new ESDial(ac.paramNames[i]));
//        dials[i]->addListener(&editor);
        addAndMakeVisible(dials[i]);
        sliderAttachments.add(new SliderAttachment(vts, ac.name + ac.paramNames[i], dials[i]->getSlider()));
    }
}

ESModule::~ESModule()
{

}

void ESModule::setBounds (float x, float y, float w, float h)
{
    Rectangle<float> newBounds (x, y, w, h);
    setBounds(newBounds);
}

void ESModule::setBounds (Rectangle<float> newBounds)
{
    Component::setBounds(newBounds.toNearestInt());
    float x = newBounds.getX();
    float w = newBounds.getWidth() / 5.f;
    float dialWidth = w * 0.6f;
    
    float labelWidth = w * 0.9f;
    float labelHeight = dialWidth * 0.35;
    
    for (int i = 0; i < ac.paramNames.size(); ++i)
    {
        dials[i]->setBounds(newBounds.withPosition(0, 0));
        dials[i]->setSliderBounds(x + w*i + w*0.2f, 0, dialWidth, dialWidth * 1.35f);
        dials[i]->setLabelBounds(dials[i]->getSlider().getX() + dialWidth*0.5f - labelWidth*0.5f,
                                 dials[i]->getSlider().getY() + dialWidth*1.325f,
                                 labelWidth, labelHeight);
    }
}
