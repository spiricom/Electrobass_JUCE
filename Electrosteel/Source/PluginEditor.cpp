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
        String n = i < NUM_GENERIC_MACROS ? "M" + String(i+1) :
        cUniqueMacroNames[i-NUM_GENERIC_MACROS];
        
        macroDials.add(new ESDial(*this, n, n, true, false));
        sliderAttachments.add(new SliderAttachment(vts, n, macroDials[i]->getSlider()));
        tab1.addAndMakeVisible(macroDials[i]);
    }
    currentMappingSource = nullptr;
    macroBorder.setFill(Colours::darkgrey);
    tab1.addAndMakeVisible(macroBorder);
    
    midiKeySource =
    std::make_unique<MappingSource>(*this, *processor.midiKeySource, "MIDI Pitch");
    tab1.addAndMakeVisible(midiKeySource.get());
    
    midiKeyRangeSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    midiKeyRangeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 10, 10);
    midiKeyRangeSlider.setRange(0, 127, 1);
    midiKeyRangeSlider.addListener(this);
    tab1.addAndMakeVisible(midiKeyRangeSlider);
    
    midiKeyMinLabel.setLookAndFeel(&laf);
    midiKeyMinLabel.setEditable(true);
    midiKeyMinLabel.setJustificationType(Justification::centred);
    midiKeyMinLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    midiKeyMinLabel.addListener(this);
    tab1.addAndMakeVisible(midiKeyMinLabel);
    
    midiKeyMaxLabel.setLookAndFeel(&laf);
    midiKeyMaxLabel.setEditable(true);
    midiKeyMaxLabel.setJustificationType(Justification::centred);
    midiKeyMaxLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    midiKeyMaxLabel.addListener(this);
    tab1.addAndMakeVisible(midiKeyMaxLabel);
    
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
        pitchBendSliders[i]->setInterceptsMouseClicks(false, false);
//        pitchBendSliders[i]->setLookAndFeel(&laf);
//        pitchBendSliders[i]->setColour(Slider::trackColourId, Colours::lightgrey);
        pitchBendSliders[i]->setColour(Slider::backgroundColourId, Colours::black);
//        pitchBendSliders[i]->setTextValueSuffix("m2");
        pitchBendSliders[i]->addListener(this);
        tab1.addAndMakeVisible(pitchBendSliders[i]);
        
        sliderAttachments.add(new SliderAttachment(vts, "PitchBend" + String(i),
                                                   *pitchBendSliders[i]));
    }
    
    mpeToggle.setButtonText("MPE");
    mpeToggle.addListener(this);
    tab1.addAndMakeVisible(mpeToggle);
    
    pedalToggle.setButtonText("Pedal volume control");
    pedalToggle.addListener(this);
    tab1.addAndMakeVisible(pedalToggle);
    
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
    
    update();
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
    
    midiKeyRangeSlider.setLookAndFeel(nullptr);
    midiKeyMinLabel.setLookAndFeel(nullptr);
    midiKeyMaxLabel.setLookAndFeel(nullptr);
    
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
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscModules[i]->setBounds(-1, (120*s*i)-i-1, 540*s+1, 120*s);
    }
    
    seriesLabel.setBounds(540*s-1, 0*s, 60*s, 25*s);
    parallelLabel.setBounds(840*s, 0*s, 60*s, 25*s);
    seriesParallelSlider.setBounds(540*s-1, 20*s, 360*s, 30*s);
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filterModules[i]->setBounds(540*s-1, 50*s+(155*s*i)-i-1, 360*s+2, 155*s);
    }

    envsAndLFOs.setBounds(-1, oscModules.getLast()->getBottom()-1, 540*s+1, 160*s);
    envsAndLFOs.setIndent(10*s);
    envsAndLFOs.setTabBarDepth(25*s);
    
    outputModule->setBounds(540*s-1, oscModules.getLast()->getBottom(), 360*s+2, 160*s-2);
    pedalToggle.setBounds(805, 500, 130, 30);
    
    const float knobSize = 40.0f*s;
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        int gap = i >= 16 ? 11*s : 0;
        macroDials[i]->setBounds(6*s + (knobSize+2)*i + gap, 523*s, knobSize, knobSize*1.8f);
        if (i == 16)
        {
            macroBorder.setRectangle(Rectangle<float>(6*s + (knobSize+2)*i + 3,
                                                      outputModule->getBottom(), 1, 68));
        }
    }
    
    int align = 78*s;
    int x = 900*s - 10*align;
    int y = 580*s;
    
    midiKeySource->setBounds(30, y+1 + 2, x-60, 24*s - 8);
    midiKeyMinLabel.setBounds(0, y, 30, 24*s - 4);
    midiKeyMaxLabel.setBounds(x-30, y, 30, 24*s - 4);
    midiKeyRangeSlider.setBounds(0, y + 24*s - 4, x, 11*s + 4);
    
    copedentButtons[1]->setBounds(x, y-1, align, 35*s);
    for (int i = 2; i < CopedentColumnNil; ++i)
    {
        copedentButtons[i]->setBounds(copedentButtons[i-1]->getRight(), y-1, align, 35*s);
    }
    
    pitchBendSliders[0]->setBounds(0, copedentButtons[1]->getBottom(), x, 24*s);
    
    int r = (10*align) % 12;
    int w = (10*align) / 12;
    y = height-35*s+2;
    mpeToggle.setBounds(6*s, y, x-w-5*s, 35*s);
    channelStringButtons[0]->setBounds(x-w, y, w, 35*s);
    for (int i = 1; i < NUM_CHANNELS; ++i)
    {
        pitchBendSliders[i]->setBounds(channelStringButtons[i-1]->getRight(),
                                       copedentButtons[1]->getBottom(),
                                       w + (r > 0 ? 1 : 0), 24*s);
        channelStringButtons[i]->setBounds(channelStringButtons[i-1]->getRight(), y,
                                           w + (r-- > 0 ? 1 : 0), 35*s);
    }
    
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
    
    if (slider == &midiKeyRangeSlider)
    {
        updateMidiKeyRangeSlider(midiKeyRangeSlider.getMinValue(),
                                 midiKeyRangeSlider.getMaxValue());
    }
}

void ESAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == nullptr) return;
    
    if (TextButton* tb = dynamic_cast<TextButton*>(button))
    {
        if (channelStringButtons.contains(tb))
        {
            updateChannelStringButton(channelStringButtons.indexOf(tb), 1);
        }
    }
    else if (ToggleButton* tb = dynamic_cast<ToggleButton*>(button))
    {
        if (tb == &mpeToggle)
        {
            updateMPEToggle(tb->getToggleState());
        }
        else if (tb == &pedalToggle)
        {
            updatePedalToggle(tb->getToggleState());
        }
    }
}

void ESAudioProcessorEditor::labelTextChanged(Label* label)
{
    if (label == nullptr) return;
    
    if (label == &midiKeyMinLabel || label == &midiKeyMaxLabel)
    {
        updateMidiKeyRangeSlider(midiKeyMinLabel.getText().getIntValue(),
                                 midiKeyMaxLabel.getText().getIntValue());
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

void ESAudioProcessorEditor::update()
{
    updatePedalToggle(processor.pedalControlsMaster);
    updateMPEToggle(processor.getMPEMode());
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        updateChannelStringButton(i, 0);
    }
    updateMidiKeyRangeSlider(processor.midiKeyMin, processor.midiKeyMax);
}

void ESAudioProcessorEditor::updatePedalToggle(bool state)
{
    processor.pedalControlsMaster = state;
    pedalToggle.setToggleState(state, dontSendNotification);
}

void ESAudioProcessorEditor::updateMPEToggle(bool state)
{
    processor.setMPEMode(state);
    mpeToggle.setToggleState(state, dontSendNotification);
    for (auto b : channelStringButtons)
    {
        b->setAlpha(state ? 1.f : 0.5f);
        b->setInterceptsMouseClicks(state, state);
    }
}

void ESAudioProcessorEditor::updateChannelStringButton(int whichButton, int inc)
{
    int ch = whichButton+1;
    processor.channelToString[ch] =
    (processor.channelToString[ch] + inc) % (NUM_STRINGS+1);
    
    String stringText = "No String";
    if (processor.channelToString[ch] > 0)
    {
        stringText = "S" + String(processor.channelToString[ch]);
    }
    channelStringButtons[whichButton]->setButtonText("Ch" + String(ch) + "|" + stringText);
}

void ESAudioProcessorEditor::updateMidiKeyRangeSlider(int min, int max)
{
    processor.midiKeyMin = min;
    processor.midiKeyMax = max;
    midiKeyRangeSlider.setMinAndMaxValues(processor.midiKeyMin, processor.midiKeyMax,
                                          dontSendNotification);
    midiKeyMinLabel.setText(String(processor.midiKeyMin), dontSendNotification);
    midiKeyMaxLabel.setText(String(processor.midiKeyMax), dontSendNotification);
}
