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
tabs(TabbedButtonBar::Orientation::TabsAtTop),
keyboard(p.keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard),
envsAndLFOs(TabbedButtonBar::TabsAtTop),
copedentTable(processor.copedentArray),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain.get())),
chooser("Select a .wav file to load...", {}, "*.wav")
{
    Typeface::Ptr tp = Typeface::createSystemTypefaceFor(BinaryData::EuphemiaCAS_ttf,
                                                         BinaryData::EuphemiaCAS_ttfSize);
    euphemia = Font(tp);
    
    logo = Drawable::createFromImageData (BinaryData::logo_large_svg, BinaryData::logo_large_svgSize);
    addAndMakeVisible(logo.get());
    synderphonicsLabel.setText("SNYDERPHONICS", dontSendNotification);
    synderphonicsLabel.setJustificationType(Justification::topLeft);
    synderphonicsLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.9f));
    addAndMakeVisible(synderphonicsLabel);
    electrosteelLabel.setText("ELECTROSTEEL", dontSendNotification);
    electrosteelLabel.setJustificationType(Justification::topLeft);
    electrosteelLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.9f));
    addAndMakeVisible(electrosteelLabel);
    
    setWantsKeyboardFocus(true);
    
    getTopLevelComponent()->addKeyListener(this);
    
    //==============================================================================
    // TAB1 ========================================================================
    addAndMakeVisible(tab1);
    
    masterDial = std::make_unique<ESDial>(*this, "Master");
    sliderAttachments.add(new SliderAttachment(vts, "Master", masterDial->getSlider()));
    tab1.addAndMakeVisible(masterDial.get());
    
    ampDial = std::make_unique<ESDial>(*this, "Amp", processor.voiceAmpParams);
    sliderAttachments.add(new SliderAttachment(vts, "Amp", ampDial->getSlider()));
    tab1.addAndMakeVisible(ampDial.get());
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
        ccDials.add(new ESDial(*this, "CC" + String(i+1), Colours::red,
                               processor.ccParams[i]->getValuePointer(), 1, false));
        sliderAttachments.add(new SliderAttachment(vts, "CC" + String(i+1), ccDials[i]->getSlider()));
        tab1.addAndMakeVisible(ccDials[i]);
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
        tab1.addAndMakeVisible(channelSelection[i]);
        
        pitchBendSliders.add(new Slider());
        pitchBendSliders[i]->setLookAndFeel(&laf);
        pitchBendSliders[i]->setTextValueSuffix(" m2");
        pitchBendSliders[i]->addListener(this);
        tab1.addAndMakeVisible(pitchBendSliders[i]);
        
        sliderAttachments.add(new SliderAttachment(vts, "PitchBendCh" + String(i+1),
                                                   *pitchBendSliders[i]));
    }
    channelSelection[0]->setButtonText("1 (Global)");
    channelSelection[1]->setToggleState(true, sendNotification);
    
    keyboard.setAvailableRange(21, 108);
    keyboard.setOctaveForMiddleC(4);
    tab1.addAndMakeVisible(&keyboard);
    
    modules.add(new ESModule(*this, vts, *processor.sposcs[0]));
    modules.add(new ESModule(*this, vts, *processor.lps[0]));
    
    envsAndLFOs.setLookAndFeel(&laf);
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        String name = "Env" + String(i+1);
        envsAndLFOs.addTab(" ", Colours::black,
                    new ESModule(*this, vts, *processor.envs[i]), true);
        envsAndLFOs.setColour(TabbedComponent::outlineColourId, Colours::darkgrey);
        
        TabbedButtonBar& bar = envsAndLFOs.getTabbedButtonBar();
        bar.getTabButton(i)
        ->setExtraComponent(new MappingSource(*this, name, processor.envs[i]->getValuePointer(),
                                              NUM_VOICES, false, Colours::cyan),
                            TabBarButton::ExtraComponentPlacement::afterText);
        bar.setColour(TabbedButtonBar::tabOutlineColourId, Colours::darkgrey);
        bar.setColour(TabbedButtonBar::frontOutlineColourId, Colours::darkgrey);
        for (int i = 0; i < envsAndLFOs.getTabbedButtonBar().getNumTabs(); ++i)
        {
            bar.getTabButton(i)->setAlpha(i == 0 ? 1.0f : 0.5f);
        }
    }
    for (int i = 0; i < NUM_LFOS; ++i)
    {
        String name = "LFO" + String(i+1);
        envsAndLFOs.addTab(" ", Colours::black,
                           new ESModule(*this, vts, *processor.lfos[i]), true);
        envsAndLFOs.setColour(TabbedComponent::outlineColourId, Colours::darkgrey);
        
        TabbedButtonBar& bar = envsAndLFOs.getTabbedButtonBar();
        bar.getTabButton(i + NUM_ENVS)
        ->setExtraComponent(new MappingSource(*this, name, processor.lfos[i]->getValuePointer(),
                                              NUM_VOICES, true, Colours::lime),
                            TabBarButton::ExtraComponentPlacement::afterText);
        bar.setColour(TabbedButtonBar::tabOutlineColourId, Colours::darkgrey);
        bar.setColour(TabbedButtonBar::frontOutlineColourId, Colours::darkgrey);
        for (int i = 0; i < envsAndLFOs.getTabbedButtonBar().getNumTabs(); ++i)
        {
            bar.getTabButton(i)->setAlpha(i == 0 ? 1.0f : 0.5f);
        }
    }

    tab1.addAndMakeVisible(&envsAndLFOs);
    tab1.addAndMakeVisible(modules[0]);
    tab1.addAndMakeVisible(modules[1]);
    
    MappingSource* env4 =
    dynamic_cast<MappingSource*>(envsAndLFOs.getTabbedButtonBar().getTabButton(3)->getExtraComponent());
    ampDial->getTarget(0)->setMapping(env4, 1.0f, HookAdd);
    
    MappingSource* env1 =
    dynamic_cast<MappingSource*>(envsAndLFOs.getTabbedButtonBar().getTabButton(0)->getExtraComponent());
    modules[1]->getDial(LowpassCutoff)->getTarget(0)->setMapping(env1, 5000.0f, HookAdd);
    
    for (int i = 0; i < CopedentColumnNil; ++i)
    {
        copedentButtons.add(new TextButton(cCopedentColumnNames[i]));
        copedentButtons[i]->setClickingTogglesState(true);
        copedentButtons[i]->setLookAndFeel(&laf);
        if (i != 0)
        {
            tab1.addAndMakeVisible(copedentButtons[i]);
            buttonAttachments.add(new ButtonAttachment(vts, cCopedentColumnNames[i],
                                                       *copedentButtons[i]));
        }
    }
    
    //==============================================================================
    // TAB2 ========================================================================
    addAndMakeVisible(tab2);
    
    tab2.addAndMakeVisible(copedentTable);
    
    exportButton.setButtonText("Export .xml");
    exportButton.setLookAndFeel(&laf);
    exportButton.onClick = [this] { copedentTable.exportXml(); };
    tab2.addAndMakeVisible(exportButton);
    
    importButton.setButtonText("Import .xml");
    importButton.setLookAndFeel(&laf);
    importButton.onClick = [this] { copedentTable.importXml(); };
    tab2.addAndMakeVisible(importButton);
    
    sendOutButton.setButtonText("Send via MIDI");
    sendOutButton.setLookAndFeel(&laf);
    sendOutButton.onClick = [this] { processor.sendCopedentMidiMessage(); };
    tab2.addAndMakeVisible(sendOutButton);
    
    //==============================================================================
    
    tabs.addTab("Synth", Colours::black, &tab1, false);
    tabs.addTab("Copedent", Colours::black, &tab2, false);
    addAndMakeVisible(&tabs);
    
    setSize(EDITOR_WIDTH * processor.editorScale, EDITOR_HEIGHT * processor.editorScale);
    
    constrain->setFixedAspectRatio(EDITOR_WIDTH / EDITOR_HEIGHT);
    
    addAndMakeVisible(*resizer);
    resizer->setAlwaysOnTop(true);
    
    versionLabel.setJustificationType(Justification::centred);
    versionLabel.setBorderSize(BorderSize<int>(2));
    versionLabel.setText("v" + String(ProjectInfo::versionString), dontSendNotification);
    versionLabel.setColour(Label::ColourIds::textColourId, Colours::lightgrey);
    addAndMakeVisible(versionLabel);
    
//    addAndMakeVisible(&container);

    startTimerHz(30);
}

ESAudioProcessorEditor::~ESAudioProcessorEditor()
{
//    masterDial->setLookAndFeel(nullptr);
//    ampDial->setLookAndFeel(nullptr);
//    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
//    {
//        ccDials[i]->setLookAndFeel(nullptr);
//    }
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        channelSelection[i]->setLookAndFeel(nullptr);
        pitchBendSliders[i]->setLookAndFeel(nullptr);
    }
    envsAndLFOs.setLookAndFeel(nullptr);
    for (int i = 0; i < CopedentColumnNil; ++i)
        copedentButtons[i]->setLookAndFeel(nullptr);
    exportButton.setLookAndFeel(nullptr);
    importButton.setLookAndFeel(nullptr);
    sendOutButton.setLookAndFeel(nullptr);
}

//==============================================================================
void ESAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(0,0), Colour(10, 10, 10), juce::Point<float>(0, getHeight()), false));
    g.setColour(Colours::black);
    g.fillRect(0, 0, getWidth(), getHeight());
    
//    Rectangle<float> panelArea = getLocalBounds().toFloat();
//    panelArea.reduce(getWidth()*0.025f, getHeight()*0.01f);
//    panelArea.removeFromBottom(getHeight()*0.03f);
//    panel->drawWithin(g, panelArea, RectanglePlacement::centred, 1.0f);
    
    g.fillRect(getWidth() * 0.25f, getHeight() * 0.25f, getWidth() * 0.6f, getHeight() * 0.5f);
    g.fillRect(getWidth() * 0.25f, getHeight() * 0.75f, getWidth() * 0.2f, getHeight() * 0.15f);
}

void ESAudioProcessorEditor::resized()
{
    int width = getWidth();
//    int height = getHeight();
    
    float s = width / EDITOR_WIDTH;
    processor.editorScale = s;
    
    tabs.setBounds(getLocalBounds().expanded(1));
    tabs.setTabBarDepth(30*s);
    
    //==============================================================================
    // TAB1 ========================================================================
    const float knobSize = 50.0f*s;
    const float masterSize = knobSize * 1.5f;
    
    masterDial->setBounds(550*s, 10*s, masterSize, masterSize*1.5f);
    ampDial->setBounds(450*s, 10*s, masterSize, masterSize*1.5f);
    
    for (int i = 0; i < NUM_GLOBAL_CC; ++i)
    {
//        ccDials[i]->setBounds(450*s + 90*s*i, 460*s, knobSize, knobSize*1.5f);
        ccDials[i]->setBounds(6.0f*s + 56.0f*s*i, 470.0f*s, knobSize, knobSize*1.4f);
    }
    
    modules[0]->setBounds(0, 10*s, 530*s, 110*s);
    modules[1]->setBounds(0, 120*s, 530*s, 110*s);
    
    copedentButtons[1]->setBounds(getWidth()-60*s, 0, 60*s+1, 30*s);
    for (int i = 2; i < CopedentColumnNil; ++i)
    {
        copedentButtons[i]->setBounds(getWidth()-60*s, copedentButtons[i-1]->getBottom(),
                                      60*s+1, 30*s);
    }
    
    envsAndLFOs.setBounds(350*s, copedentButtons.getLast()->getBottom(), width - 350*s + 2, 160*s);
    envsAndLFOs.setIndent(10*s);
    envsAndLFOs.setTabBarDepth(25*s);
    
    resizedChannelSelection();
    
    keyboard.setBoundsRelative(0.f, 0.86f, 1.0f, 0.14f);
    keyboard.setKeyWidth(width / 52.0f);
    
    //==============================================================================
    // TAB2 ========================================================================
    
    copedentTable.setBoundsRelative(0.05f, 0.1f, 0.9f, 0.7f);
    exportButton.setBoundsRelative(0.05f, 0.85f, 0.15f, 0.05f);
    importButton.setBoundsRelative(0.21f, 0.85f, 0.15f, 0.05f);
    sendOutButton.setBoundsRelative(0.37f, 0.85f, 0.15f, 0.05f);

    //==============================================================================
    
    versionLabel.setBounds(width*0.95, 0, width * 0.05f, tabs.getTabBarDepth());
    versionLabel.setFont(euphemia.withHeight(20*s));
    
    int logoLeft = tabs.getTabbedButtonBar().getTabButton(1)->getRight() + 90*s;
    Rectangle<float> logoArea (logoLeft, 0, 98*s, tabs.getTabBarDepth());
    logo->setTransformToFit (logoArea,
                             RectanglePlacement::xLeft +
                             RectanglePlacement::yTop +
                             RectanglePlacement::fillDestination);
    synderphonicsLabel.setBounds(logoLeft+50*s, -5*s, 220*s, 34*s);
    synderphonicsLabel.setFont(euphemia.withHeight(34*s));
    electrosteelLabel.setBounds(synderphonicsLabel.getRight(), -5*s, 300*s, 34*s);
    electrosteelLabel.setFont(euphemia.withHeight(34*s).withStyle(3));
    
    
    float r = EDITOR_WIDTH / EDITOR_HEIGHT;
    constrain->setSizeLimits(200, 200/r, 800*r, 800);
    resizer->setBounds(getWidth()-12, getHeight()-12, 12, 12);
    
//    container.setBounds(getLocalBounds());
}

void ESAudioProcessorEditor::resizedChannelSelection()
{
    int width = getWidth();
    float s = width / EDITOR_WIDTH;
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        float yf = 547;
        if (channelSelection[i]->getToggleState()) yf -= 4;
        float offset = width * 0.07f;
        float w = width * 0.04f;
        if (i == 0)
        {
            w += offset;
            offset = 0.0f;
        }
        Rectangle<float> bounds ((width * 0.04f) * i + offset, yf*s, w, 42*s);
        channelSelection[i]->setBounds(Rectangle<int>::leftTopRightBottom ((bounds.getX()),
                                                                           (bounds.getY()),
                                                                           (bounds.getRight()),
                                                                           (bounds.getBottom())));
    }
    
    int w = width - channelSelection[NUM_CHANNELS-1]->getRight();
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        pitchBendSliders[i]->setBounds(channelSelection[NUM_CHANNELS-1]->getRight(), 543*s, w, 40*s);
    }
}

void ESAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == nullptr) return;
}

void ESAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == nullptr) return;
    
    resizedChannelSelection();
    
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
}

void ESAudioProcessorEditor::buttonStateChanged(Button* button)
{
    if (button == nullptr) return;
    
    if (button->getState() == Button::ButtonState::buttonNormal)
    {
    }
    else if (button->getState() == Button::ButtonState::buttonDown)
    {
    }
    else if (button->getState() == Button::ButtonState::buttonOver)
    {
    }
}

void ESAudioProcessorEditor::mouseDown (const MouseEvent &event)
{
    if (MappingSource* ms = dynamic_cast<MappingSource*>(event.originalComponent->getParentComponent()))
    {
        currentMappingSource = ms;
        if (TabBarButton* tbb = dynamic_cast<TabBarButton*>(ms->getParentComponent()))
        {
            if (&ms->button != event.originalComponent)
            {
                tbb->triggerClick();
            }
        }
        startDragging(ms->getName(), ms);
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
                if (i + d > NUM_CHANNELS) return false;
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

ESModule::ESModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts, AudioComponent& ac) :
editor(editor),
vts(vts),
ac(ac)
{
    setInterceptsMouseClicks(false, true);
    
    for (int i = 0; i < ac.paramNames.size(); i++)
    {
        dials.add(new ESDial(editor, ac.paramNames[i], ac.getParameter(i)));
        addAndMakeVisible(dials[i]);
        sliderAttachments.add(new SliderAttachment(vts, ac.name + ac.paramNames[i], dials[i]->getSlider()));
    }
}

ESModule::~ESModule()
{

}

void ESModule::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    float x = area.getX();
    float w = area.getWidth() / 5.f;
    float dialWidth = w * 0.6f;
    
    for (int i = 0; i < ac.paramNames.size(); ++i)
    {
        dials[i]->setBounds(x + w*i + w*0.2f, 0, dialWidth, dialWidth * 1.6f);
//        dials[i]->setSliderBounds(x + w*i + w*0.2f, 0, dialWidth, dialWidth * 1.35f);
//        dials[i]->setLabelBounds(dials[i]->getSlider().getX() + dialWidth*0.5f - labelWidth*0.5f,
//                                 dials[i]->getSlider().getY() + dialWidth*1.325f,
//                                 labelWidth, labelHeight);
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

ESDial* ESModule::getDial (String param)
{
    int i = ac.paramNames.indexOf(param);
    if (i < 0) return nullptr;
    return dials[i];
}
