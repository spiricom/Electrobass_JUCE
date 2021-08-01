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
    
    logo = Drawable::createFromImageData (BinaryData::logo_large_svg,
                                          BinaryData::logo_large_svgSize);
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
    
    currentMappingSource = nullptr;
    uniqueMacroComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(uniqueMacroComponent);
    
    for (int i = 0; i < NUM_MACROS; ++i)
    {
        String n = i < NUM_GENERIC_MACROS ? "M" + String(i+1) :
        cUniqueMacroNames[i-NUM_GENERIC_MACROS];
        
        macroDials.add(new ESDial(*this, n, n, true, false));
        sliderAttachments.add(new SliderAttachment(vts, n, macroDials[i]->getSlider()));
        
        if (i < NUM_GENERIC_MACROS) tab1.addAndMakeVisible(macroDials[i]);
        else uniqueMacroComponent.addAndMakeVisible(macroDials[i]);
    }
    
    midiKeyComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(midiKeyComponent);
    
    midiKeySource = std::make_unique<MappingSource>(*this, *processor.midiKeySource, 
                                                    "MIDI Key In");
    midiKeyComponent.addAndMakeVisible(midiKeySource.get());
    
    midiKeyRangeSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    midiKeyRangeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 10, 10);
    midiKeyRangeSlider.setRange(0, 127, 1);
    midiKeyRangeSlider.addListener(this);
    midiKeyComponent.addAndMakeVisible(midiKeyRangeSlider);
    
    midiKeyMinLabel.setLookAndFeel(&laf);
    midiKeyMinLabel.setEditable(true);
    midiKeyMinLabel.setJustificationType(Justification::centred);
    midiKeyMinLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    midiKeyMinLabel.addListener(this);
    midiKeyComponent.addAndMakeVisible(midiKeyMinLabel);
    
    midiKeyMaxLabel.setLookAndFeel(&laf);
    midiKeyMaxLabel.setEditable(true);
    midiKeyMaxLabel.setJustificationType(Justification::centred);
    midiKeyMaxLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    midiKeyMaxLabel.addListener(this);
    midiKeyComponent.addAndMakeVisible(midiKeyMaxLabel);

    velocityComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(velocityComponent);

    velocitySource = std::make_unique<MappingSource>(*this, *processor.velocitySource, 
                                                     "Velocity In");
    velocityComponent.addAndMakeVisible(velocitySource.get());

    randomComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(randomComponent);
    
    randomSource = std::make_unique<MappingSource>(*this, *processor.randomSource, 
                                                   "Random on Attack");
    randomComponent.addAndMakeVisible(randomSource.get());
    
    randomValueLabel.setLookAndFeel(&laf);
    randomValueLabel.setEditable(false);
    randomValueLabel.setJustificationType(Justification::centred);
    randomValueLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    randomValueLabel.addListener(this);
    randomComponent.addAndMakeVisible(randomValueLabel);
    
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        String n = "Str" + String(i);
        if (i == 0) n = "PB+CCs";
        stringActivityButtons.add(new TextButton(n));
        stringActivityButtons[i]->setLookAndFeel(&laf);
        stringActivityButtons[i]->setConnectedEdges(Button::ConnectedOnLeft &
                                                     Button::ConnectedOnRight);
        stringActivityButtons[i]->setInterceptsMouseClicks(false, false);
        stringActivityButtons[i]->setClickingTogglesState(false);//(true);
        stringActivityButtons[i]->addListener(this);
        tab1.addAndMakeVisible(stringActivityButtons[i]);
        
        pitchBendSliders.add(new Slider());
        pitchBendSliders[i]->setSliderStyle(Slider::SliderStyle::LinearBar);
        pitchBendSliders[i]->setInterceptsMouseClicks(false, false);
//        pitchBendSliders[i]->setLookAndFeel(&laf);
//        pitchBendSliders[i]->setColour(Slider::trackColourId, Colours::lightgrey);
        pitchBendSliders[i]->setColour(Slider::backgroundColourId, Colours::black);
        pitchBendSliders[i]->setColour(Slider::textBoxOutlineColourId, Colours::grey);
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
    
    //==============================================================================
    // TAB2 ========================================================================
    addAndMakeVisible(tab2);
    
    tab2.addAndMakeVisible(copedentTable);
    
    //==============================================================================
    // TAB3 ========================================================================
    addAndMakeVisible(tab3);
    
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        if (i < NUM_MACROS)
        {
            String n = "M" + String(i+1);
            if (i >= NUM_GENERIC_MACROS) n = cUniqueMacroNames[i-NUM_GENERIC_MACROS];
            macroControlLabels.add(new Label());
            macroControlLabels.getLast()->setText(n + " CC#", dontSendNotification);
            macroControlLabels.getLast()->setJustificationType(Justification::centredRight);
            macroControlLabels.getLast()->setLookAndFeel(&laf);
            tab3.addAndMakeVisible(macroControlLabels.getLast());
        }
        
        macroControlEntries.add(new Label());
        macroControlEntries.getLast()->setLookAndFeel(&laf);
        macroControlEntries.getLast()->setEditable(true);
        macroControlEntries.getLast()->setJustificationType(Justification::centred);
        macroControlEntries.getLast()->setColour(Label::backgroundColourId,
                                                Colours::darkgrey.withBrightness(0.2f));
        macroControlEntries.getLast()->addListener(this);
        tab3.addAndMakeVisible(macroControlEntries.getLast());
    }
    
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        String n = "String " + String(i);
        if (i == 0) n = "Global Pitch Bend & CCs";
        stringChannelLabels.add(new Label());
        stringChannelLabels.getLast()->setText(n + " Ch#", dontSendNotification);
        stringChannelLabels.getLast()->setJustificationType(Justification::centredRight);
        stringChannelLabels.getLast()->setLookAndFeel(&laf);
        tab3.addAndMakeVisible(stringChannelLabels.getLast());
        
        stringChannelEntries.add(new Label());
        stringChannelEntries.getLast()->setLookAndFeel(&laf);
        stringChannelEntries.getLast()->setEditable(true);
        stringChannelEntries.getLast()->setJustificationType(Justification::centred);
        stringChannelEntries.getLast()->setColour(Label::backgroundColourId,
                                                 Colours::darkgrey.withBrightness(0.2f));
        stringChannelEntries.getLast()->addListener(this);
        tab3.addAndMakeVisible(stringChannelEntries.getLast());
    }
    
    //==============================================================================
    
    tabs.addTab("Synth", Colours::black, &tab1, false);
    tabs.addTab("Copedent", Colours::black, &tab2, false);
    tabs.addTab("Control", Colours::black, &tab3, false);
    tabs.getTabbedButtonBar().getTabButton(0)->addListener(this);
    tabs.getTabbedButtonBar().getTabButton(1)->addListener(this);
    tabs.getTabbedButtonBar().getTabButton(2)->addListener(this);
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
        stringActivityButtons[i]->setLookAndFeel(nullptr);
        pitchBendSliders[i]->setLookAndFeel(nullptr);
    }
    
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        if (i < NUM_MACROS) macroControlLabels[i]->setLookAndFeel(nullptr);
        macroControlEntries[i]->setLookAndFeel(nullptr);
    }
    
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        stringChannelLabels[i]->setLookAndFeel(nullptr);
        stringChannelEntries[i]->setLookAndFeel(nullptr);
    }
    
    seriesLabel.setLookAndFeel(nullptr);
    parallelLabel.setLookAndFeel(nullptr);
    envsAndLFOs.setLookAndFeel(nullptr);
    
    sendOutButton.setLookAndFeel(nullptr);
    
    midiKeyRangeSlider.setLookAndFeel(nullptr);
    midiKeyMinLabel.setLookAndFeel(nullptr);
    midiKeyMaxLabel.setLookAndFeel(nullptr);
    
    randomValueLabel.setLookAndFeel(nullptr);
    
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
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        macroDials[i]->setBounds(6*s + (knobSize+2)*i, 523*s, knobSize, knobSize*1.8f);
    }
    
    uniqueMacroComponent.setBounds(6*s + (knobSize+2)*NUM_GENERIC_MACROS + 3,
                                   outputModule->getBottom()-1, 300, 69);
    
    for (int i = NUM_GENERIC_MACROS; i < NUM_MACROS; ++i)
    {
        macroDials[i]->setBounds(11*s + (knobSize+2)*(i-NUM_GENERIC_MACROS) - 3,
                                 523*s - uniqueMacroComponent.getY(),
                                 knobSize, knobSize*1.8f);
    }
    
    int align = 78*s;
    int x = 900*s - 10*align;
    int y = 582*s;
    
    midiKeyComponent.setBounds(-1, y-2, 657, 33);
    midiKeySource->setBounds(5, 7, x-40, 22*s - 4);
    midiKeyMinLabel.setBounds(midiKeySource->getRight()+4, 7, 40, 22*s - 4);
    midiKeyRangeSlider.setBounds(midiKeyMinLabel.getRight(), 7, 468, 22*s - 4);
    midiKeyMaxLabel.setBounds(midiKeyRangeSlider.getRight(), 7, 40, 22*s - 4);

    velocityComponent.setBounds(midiKeyComponent.getRight() - 1, y - 2, 102, 33);
    velocitySource->setBounds(5, 7, 91, 22 * s - 4);

    randomComponent.setBounds(velocityComponent.getRight() - 1, y - 2, x + 60, 33);
    randomSource->setBounds(5, 7, x+2, 22*s - 4);
    randomValueLabel.setBounds(randomSource->getRight()+4, 7, 40, 22*s - 4);
        
    int r = (10*align) % 12;
    int w = (10*align) / 12;
    y = height-35*s+2;
    mpeToggle.setBounds(6*s, y, x-w-5*s, 35*s);
    pitchBendSliders[0]->setBounds(0, midiKeyComponent.getBottom()-1, x, 27*s);
    stringActivityButtons[0]->setBounds(x-w, y, w, 35*s);
    for (int i = 1; i < NUM_CHANNELS; ++i)
    {
        pitchBendSliders[i]->setBounds(pitchBendSliders[i-1]->getRight(),
                                       midiKeyComponent.getBottom()-1,
                                       w + (r > 0 ? 1 : 0), 26*s);
        stringActivityButtons[i]->setBounds(stringActivityButtons[i-1]->getRight(), y,
                                           w + (r-- > 0 ? 1 : 0), 35*s);
    }
    
    //    keyboard.setBoundsRelative(0.f, 0.86f, 1.0f, 0.14f);
    //    keyboard.setKeyWidth(width / 52.0f);
    
    //==============================================================================
    // TAB2 ========================================================================
    
    copedentTable.setBoundsRelative(0.05f, 0.08f, 0.9f, 0.84f);
    
    //==============================================================================
    // TAB3 ========================================================================
    
    int h = 30;
    int pad = 4;
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        stringChannelLabels[i]->setBounds(10, 30+(h+pad)*i, 300, h);
        stringChannelEntries[i]->setBounds(stringChannelLabels[i]->getRight(),
                                           stringChannelLabels[i]->getY(), 100, h);
    }
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        macroControlLabels[i]->setBounds(430, 30+(h+pad)*i, 120, h);
        macroControlEntries[i]->setBounds(macroControlLabels[i]->getRight(),
                                          macroControlLabels[i]->getY(), 100, h);
    }
    for (int i = NUM_GENERIC_MACROS; i < NUM_MACROS; ++i)
    {
        macroControlLabels[i]->setBounds(640, 30+(h+pad)*(i-NUM_GENERIC_MACROS), 120, h);
        macroControlEntries[i]->setBounds(macroControlLabels[i]->getRight(),
                                          macroControlLabels[i]->getY(), 100, h);
        if (i == NUM_MACROS - 1)
        {
            macroControlLabels[i]->setBounds(640, 30+(h+pad)*(i-NUM_GENERIC_MACROS+0.5f), 120, h);
            macroControlEntries[NUM_MACROS]->setBounds(macroControlLabels[i]->getRight(),
                                                       30+(h+pad)*NUM_UNIQUE_MACROS, 100, h);
        }
    }
    
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

    if (ToggleButton* tb = dynamic_cast<ToggleButton*>(button))
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
    
    if (button == tabs.getTabbedButtonBar().getTabButton(0))
    {
        tab1.addAndMakeVisible(mpeToggle);
        for (auto slider : pitchBendSliders) tab1.addAndMakeVisible(slider);
        for (auto button : stringActivityButtons) tab1.addAndMakeVisible(button);
    }
    else if (button == tabs.getTabbedButtonBar().getTabButton(1))
    {
        
    }
    else if (button == tabs.getTabbedButtonBar().getTabButton(2))
    {
        tab3.addAndMakeVisible(mpeToggle);
        for (auto slider : pitchBendSliders) tab3.addAndMakeVisible(slider);
        for (auto button : stringActivityButtons) tab3.addAndMakeVisible(button);
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
    else if (macroControlEntries.contains(label))
    {
        int ctrl = label->getText().getIntValue();
        updateMacroControl(macroControlEntries.indexOf(label), ctrl);
    }
    else if (stringChannelEntries.contains(label))
    {
        int ch = label->getText().getIntValue();
        updateStringChannel(stringChannelEntries.indexOf(label), ch);
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
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        stringActivityButtons[i]->setToggleState(processor.stringIsActive(i),
                                                 dontSendNotification);
    }
    updateRandomValueLabel(processor.lastRandomValue);
}

void ESAudioProcessorEditor::update()
{
    updatePedalToggle(processor.pedalControlsMaster);
    updateMPEToggle(processor.getMPEMode());
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        updateStringChannel(i, processor.stringChannels[i]);
    }
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        updateMacroControl(i, processor.macroCCNumbers[i]);
    }
    updateMidiKeyRangeSlider(processor.midiKeyMin, processor.midiKeyMax);
    updateRandomValueLabel(processor.lastRandomValue);
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
    for (int i = 1; i < pitchBendSliders.size(); ++i)
    {
        pitchBendSliders[i]->setAlpha(state ? 1.f : 0.7f);
        pitchBendSliders[i]->setEnabled(state);
    }
    for (int i = 0; i < stringChannelEntries.size(); ++i)
    {
        stringChannelEntries[i]->setAlpha(state ? 1.f : 0.7f);
        String text = "All";
        if (state) text = String(processor.stringChannels[i]);
        stringChannelEntries[i]->setText(text, dontSendNotification);
        stringChannelEntries[i]->setEnabled(state);
    }
}

void ESAudioProcessorEditor::updateStringChannel(int string, int ch)
{
    ch = jlimit(0, 16, ch);
    // Handle mapping that will be overwritten
    for (int i = 0; i < NUM_STRINGS+1; ++i)
    {
        if (processor.stringChannels[i] == ch)
        {
            processor.stringChannels[i] = 0;
            stringChannelEntries[i]->setText("", dontSendNotification);
        }
    }
    // Disable the old mapping
    processor.channelToStringMap.set(processor.stringChannels[string], -1);
    // Set the new mapping
    processor.stringChannels[string] = ch;
    processor.channelToStringMap.set(ch, string);
    // Update the text
    String text = ch > 0 ? String(processor.stringChannels[string]) : "";
    stringChannelEntries[string]->setText(text, dontSendNotification);
}

void ESAudioProcessorEditor::updateMacroControl(int macro, int ctrl)
{
    ctrl = jlimit(0, 127, ctrl);
    // Handle mapping that will be overwritten
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        if (processor.macroCCNumbers[i] == ctrl)
        {
            processor.macroCCNumbers[i] = 0;
            macroControlEntries[i]->setText("", dontSendNotification);
        }
    }
    // Disable the old mapping
    processor.ccNumberToMacroMap.set(processor.macroCCNumbers[macro], -1);
    // Set the new mapping
    processor.macroCCNumbers[macro] = ctrl;
    processor.ccNumberToMacroMap.set(ctrl, macro);
    // Update the text
    String text = ctrl > 0 ? String(processor.macroCCNumbers[macro]) : "";
    macroControlEntries[macro]->setText(text, dontSendNotification);
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

void ESAudioProcessorEditor::updateRandomValueLabel(float value)
{
    randomValueLabel.setText(String(value, 3), dontSendNotification);
}
