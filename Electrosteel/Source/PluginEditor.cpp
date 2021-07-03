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
copedentTable(processor, vts),
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
    
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        macroDials.add(new ESDial(*this, "M" + String(i+1), "M" + String(i+1), true, false));
        sliderAttachments.add(new SliderAttachment(vts, "M" + String(i+1), macroDials[i]->getSlider()));
        tab1.addAndMakeVisible(macroDials[i]);
    }
    currentMappingSource = nullptr;
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        String stringText = "No String";
        if (processor.channelToString[i+1] > 0)
        {
            stringText = "S" + String(processor.channelToString[i+1]);
        }
        channelStringButtons.add(new TextButton("Ch" + String(i+1) + "|" + stringText));
        channelStringButtons[i]->setLookAndFeel(&laf);
        channelStringButtons[i]->setConnectedEdges(Button::ConnectedOnLeft & Button::ConnectedOnRight);
        //        channelSelection[i]->setRadioGroupId(1);
        channelStringButtons[i]->setClickingTogglesState(false);//(true);
        channelStringButtons[i]->addListener(this);
        tab1.addAndMakeVisible(channelStringButtons[i]);
        
        pitchBendSliders.add(new Slider());
        pitchBendSliders[i]->setSliderStyle(Slider::SliderStyle::LinearBar);
//        pitchBendSliders[i]->setLookAndFeel(&laf);
//        pitchBendSliders[i]->setColour(Slider::trackColourId, Colours::lightgrey);
        pitchBendSliders[i]->setColour(Slider::backgroundColourId, Colours::black);
        pitchBendSliders[i]->setTextValueSuffix(" m2");
        pitchBendSliders[i]->addListener(this);
//        tab1.addAndMakeVisible(pitchBendSliders[i]);
        
        sliderAttachments.add(new SliderAttachment(vts, "PitchBend" + String(i),
                                                   *pitchBendSliders[i]));
    }
    tab1.addAndMakeVisible(pitchBendSliders[0]);
    
    bool state = processor.getMPEMode();
    mpeToggle.setButtonText("MPE");
    mpeToggle.setToggleState(processor.getMPEMode(), dontSendNotification);
    mpeToggle.addListener(this);
    tab1.addAndMakeVisible(mpeToggle);
    for (auto b : channelStringButtons)
    {
        b->setAlpha(state ? 1.f : 0.5f);
        b->setInterceptsMouseClicks(state, state);
    }
    
    keyboard.setAvailableRange(21, 108);
    keyboard.setOctaveForMiddleC(4);
    //    tab1.addAndMakeVisible(&keyboard);
    
    tab1.addAndMakeVisible(&envsAndLFOs);
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscModules.add(new OscModule(*this, vts, *processor.oscs[i]));
        tab1.addAndMakeVisible(oscModules[i]);
    }
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filterModules.add(new FilterModule(*this, vts, *processor.filt[i]));
        tab1.addAndMakeVisible(filterModules[i]);
    }
    
    seriesParallelSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    seriesParallelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 10, 10);
    tab1.addAndMakeVisible(seriesParallelSlider);
    sliderAttachments.add(new SliderAttachment(vts, "Filter Series-Parallel Mix",
                                               seriesParallelSlider));
    
    seriesLabel.setText("Ser.", dontSendNotification);
    seriesLabel.setLookAndFeel(&laf);
    tab1.addAndMakeVisible(seriesLabel);
    
    parallelLabel.setText("Par.", dontSendNotification);
    parallelLabel.setJustificationType(Justification::centredRight);
    parallelLabel.setLookAndFeel(&laf);
    tab1.addAndMakeVisible(parallelLabel);
    
    outputModule = std::make_unique<OutputModule>(*this, vts, *processor.output);
    tab1.addAndMakeVisible(outputModule.get());
    
    envsAndLFOs.setLookAndFeel(&laf);
    for (int i = 0; i < NUM_ENVS; ++i)
    {
        String paramName = "Envelope" + String(i+1);
        String displayName = "Env" + String(i+1);
        envsAndLFOs.addTab(" ", Colours::black, new EnvModule(*this, vts, *processor.envs[i]), true);
        envsAndLFOs.setColour(TabbedComponent::outlineColourId, Colours::darkgrey);
        
        TabbedButtonBar& bar = envsAndLFOs.getTabbedButtonBar();
        bar.getTabButton(i)
        ->setExtraComponent(new MappingSource(*this, *processor.getMappingSource(paramName), displayName),
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
        String paramName = "LFO" + String(i+1);
        String displayName = paramName;
        envsAndLFOs.addTab(" ", Colours::black,
                           new LFOModule(*this, vts, *processor.lfos[i]), true);
        envsAndLFOs.setColour(TabbedComponent::outlineColourId, Colours::darkgrey);
        
        TabbedButtonBar& bar = envsAndLFOs.getTabbedButtonBar();
        bar.getTabButton(i + NUM_ENVS)
        ->setExtraComponent(new MappingSource(*this, *processor.getMappingSource(paramName), displayName),
                            TabBarButton::ExtraComponentPlacement::afterText);
        bar.setColour(TabbedButtonBar::tabOutlineColourId, Colours::darkgrey);
        bar.setColour(TabbedButtonBar::frontOutlineColourId, Colours::darkgrey);
        for (int i = 0; i < envsAndLFOs.getTabbedButtonBar().getNumTabs(); ++i)
        {
            bar.getTabButton(i)->setAlpha(i == 0 ? 1.0f : 0.5f);
        }
    }
    
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
    
    //==============================================================================
    
    tabs.addTab("Synth", Colours::black, &tab1, false);
    tabs.addTab("Copedent", Colours::black, &tab2, false);
    addAndMakeVisible(&tabs);
    
    setSize(EDITOR_WIDTH * processor.editorScale, EDITOR_HEIGHT * processor.editorScale);
    
    sendOutButton.setButtonText("Send preset via MIDI");
    sendOutButton.setLookAndFeel(&laf);
    sendOutButton.onClick = [this] { processor.sendPresetMidiMessage(); };
    addAndMakeVisible(sendOutButton);
    
    constrain->setFixedAspectRatio(EDITOR_WIDTH / EDITOR_HEIGHT);
    
//    addAndMakeVisible(*resizer);
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
    //    for (int i = 0; i < NUM_MACROS; ++i)
    //    {
    //        ccDials[i]->setLookAndFeel(nullptr);
    //    }
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        channelStringButtons[i]->setLookAndFeel(nullptr);
        pitchBendSliders[i]->setLookAndFeel(nullptr);
    }
    
    seriesLabel.setLookAndFeel(nullptr);
    parallelLabel.setLookAndFeel(nullptr);
    envsAndLFOs.setLookAndFeel(nullptr);
    for (int i = 0; i < CopedentColumnNil; ++i)
    copedentButtons[i]->setLookAndFeel(nullptr);
    
    sendOutButton.setLookAndFeel(nullptr);
    
    sliderAttachments.clear();
    buttonAttachments.clear();
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
    int height = getHeight();
    
    float s = width / EDITOR_WIDTH;
    processor.editorScale = 1.05f;
    
    tabs.setBounds(getLocalBounds().expanded(1));
    tabs.setTabBarDepth(30*s);
    
    height -= tabs.getTabBarDepth();
    
    //==============================================================================
    // TAB1 ========================================================================
    const float knobSize = 50.0f*s;
    
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        macroDials[i]->setBounds(6*s + 56*s*i, 527*s, knobSize, knobSize*1.4f);
    }
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscModules[i]->setBounds(0*s, (120*s*i)-i, 540*s, 120*s);
    }
    
    seriesLabel.setBounds(540*s-1, 0*s, 60*s, 25*s);
    parallelLabel.setBounds(840*s, 0*s, 60*s, 25*s);
    seriesParallelSlider.setBounds(540*s-1, 20*s, 360*s, 30*s);
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filterModules[i]->setBounds(540*s-1, 50*s+(155*s*i)-i-1, 360*s+1, 155*s);
    }

    envsAndLFOs.setBounds(0*s, oscModules.getLast()->getBottom()-1, 540*s, 160*s);
    envsAndLFOs.setIndent(10*s);
    envsAndLFOs.setTabBarDepth(25*s);
    
    outputModule->setBounds(540*s-1, oscModules.getLast()->getBottom()-1, 360*s+1, 160*s-1);
    
    int align = 78*s;
    int x = 900*s - 10*align;
    copedentButtons[1]->setBounds(x, 600*s, align, 35*s);
    for (int i = 2; i < CopedentColumnNil; ++i)
    {
        copedentButtons[i]->setBounds(copedentButtons[i-1]->getRight(), 600*s,
                                      align, 35*s);
    }
    
    int r = (10*align) % 12;
    int w = (10*align) / 12;
    channelStringButtons[0]->setBounds(x-w, height-35*s+2, w, 35*s);
    for (int i = 1; i < NUM_CHANNELS; ++i)
    {
        channelStringButtons[i]->setBounds(channelStringButtons[i-1]->getRight(), height-35*s+2,
                                           w + (r-- > 0 ? 1 : 0), 35*s);
    }
    
//    w = width - channelStringButtons[NUM_CHANNELS-1]->getRight();
//    for (int i = 0; i < NUM_CHANNELS; ++i)
//    {
//        pitchBendSliders[i]->setBounds(channelStringButtons[NUM_CHANNELS-1]->getRight(),
//                                       height-35*s+1, w, 35*s);
//    }
    
    pitchBendSliders[0]->setBounds(0*s, 600*s, x, 35*s);
    
    mpeToggle.setBounds(6*s, height-35*s+2, x-w-5*s, 35*s);
    
    //    keyboard.setBoundsRelative(0.f, 0.86f, 1.0f, 0.14f);
    //    keyboard.setKeyWidth(width / 52.0f);
    
    //==============================================================================
    // TAB2 ========================================================================
    
    copedentTable.setBoundsRelative(0.05f, 0.08f, 0.9f, 0.84f);
    
    //==============================================================================
    
    versionLabel.setBounds(width*0.75f, 0, width * 0.05f, tabs.getTabBarDepth());
    versionLabel.setFont(euphemia.withHeight(20*s));
    
    sendOutButton.setBounds(width*0.85f, -1, width*0.15f+2, tabs.getTabBarDepth());
    
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
    
    float rt = EDITOR_WIDTH / EDITOR_HEIGHT;
    constrain->setSizeLimits(200, 200/rt, 800*rt, 800);
    resizer->setBounds(getWidth()-12, getHeight()-12, 12, 12);
    
    //    container.setBounds(getLocalBounds());
}

void ESAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == nullptr) return;
}

void ESAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == nullptr) return;
    
    if (TextButton* tb = dynamic_cast<TextButton*>(button))
    {
        if (channelStringButtons.contains(tb))
        {
            int ch = channelStringButtons.indexOf(tb) + 1;
            processor.channelToString[ch] =
            (processor.channelToString[ch] + 1) % (NUM_STRINGS+1);
            
            String stringText = "No String";
            if (processor.channelToString[ch] > 0)
            {
                stringText = "S" + String(processor.channelToString[ch]);
            }
            tb->setButtonText("Ch" + String(ch) + "|" + stringText);
            
//            keyboard.setMidiChannel(channel);
//            keyboard.setAlpha(channel > 1 ? 1.0f : 0.5f);
//            keyboard.setInterceptsMouseClicks(channel > 1, channel > 1);
            
//            for (int i = 0; i < NUM_CHANNELS; ++i)
//            {
//                pitchBendSliders[i]->setVisible(false);
//                if (i+1 == channel) pitchBendSliders[i]->setVisible(true);
//            }
        }
    }
    else if (ToggleButton* tb = dynamic_cast<ToggleButton*>(button))
    {
        if (tb == &mpeToggle)
        {
            bool state = tb->getToggleState();
            processor.setMPEMode(state);
            for (auto b : channelStringButtons)
            {
                b->setAlpha(state ? 1.f : 0.5f);
                b->setInterceptsMouseClicks(state, state);
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
    return false;
}

void ESAudioProcessorEditor::timerCallback()
{
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        channelStringButtons[i]->setToggleState(processor.midiChannelIsActive(i+1), dontSendNotification);
    }
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
