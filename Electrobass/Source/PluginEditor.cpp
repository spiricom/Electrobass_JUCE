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

ElectroAudioProcessorEditor::ElectroAudioProcessorEditor (ElectroAudioProcessor& p, AudioProcessorValueTreeState& vts) :
AudioProcessorEditor (&p),
processor (p),
vts(vts),
OSCILLOSCOPE(processor.getAudioBufferQueue()),
tabs(TabbedButtonBar::Orientation::TabsAtTop),
keyboard(p.keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard),
envsAndLFOs(TabbedButtonBar::TabsAtTop),
tuningTab(processor, vts),
fxTab(*this, processor,vts),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain.get())),
chooser("Select a .wav file to load...", {}, "*.wav")

{
    
   
    
    Typeface::Ptr tp = Typeface::createSystemTypefaceFor(BinaryData::EuphemiaCAS_ttf,
                                                         BinaryData::EuphemiaCAS_ttfSize);
    euphemia = Font(tp);
    addAndMakeVisible(tooltipWindow);
    logo = Drawable::createFromImageData (BinaryData::logo_large_svg,
                                          BinaryData::logo_large_svgSize);
    addAndMakeVisible(logo.get());
    synderphonicsLabel.setText("SNYDERPHONICS", dontSendNotification);
    synderphonicsLabel.setJustificationType(Justification::topLeft);
    synderphonicsLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.9f));
    addAndMakeVisible(synderphonicsLabel);
    ElectrobassLabel.setText("Electrobass", dontSendNotification);
    ElectrobassLabel.setJustificationType(Justification::topLeft);
    ElectrobassLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.9f));
    addAndMakeVisible(ElectrobassLabel);
    
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
        
        macroDials.add(new ElectroDial(*this, n, n, true, false));
        sliderAttachments.add(new SliderAttachment(vts, n, macroDials[i]->getSlider()));
        
        if (i < NUM_GENERIC_MACROS) tab1.addAndMakeVisible(macroDials[i]);
        else uniqueMacroComponent.addAndMakeVisible(macroDials[i]);
    }
    
    // MIDI Key source
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

    // Velocity source
    velocityComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(velocityComponent);

    velocitySource = std::make_unique<MappingSource>(*this, *processor.velocitySource,
                                                     "Velocity In");
    velocityComponent.addAndMakeVisible(velocitySource.get());

    // Random source
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
    
    // Transpose, Poly
   
    
    numVoicesLabel.setText("Voices", dontSendNotification);
    numVoicesLabel.setJustificationType(Justification::centred);
    numVoicesLabel.setLookAndFeel(&laf);
    tab1.addAndMakeVisible(numVoicesLabel);
    
    numVoicesSlider.setRange(1., (float)MAX_NUM_VOICES, 1.); //EBSPECIFIC
    numVoicesSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    numVoicesSlider.setSliderSnapsToMousePosition(false);
    numVoicesSlider.setMouseDragSensitivity(200);
    numVoicesSlider.setTextValueSuffix(""); //EBSPECIFIC
    numVoicesSlider.setLookAndFeel(&laf);
    numVoicesSlider.setColour(Slider::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    numVoicesSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    numVoicesSlider.setColour(Slider::textBoxTextColourId, Colours::gold.withBrightness(0.95f));
    numVoicesSlider.addListener(this); //EBSPECIFIC removes ability to change
    tab1.addAndMakeVisible(numVoicesSlider);
    
    transposeLabel.setText("Transpose", dontSendNotification);
    transposeLabel.setJustificationType(Justification::centred);
    transposeLabel.setLookAndFeel(&laf);
    tab1.addAndMakeVisible(transposeLabel);
    
    transposeSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    transposeSlider.setSliderSnapsToMousePosition(false);
    transposeSlider.setMouseDragSensitivity(400);
    transposeSlider.setLookAndFeel(&laf);
    transposeSlider.setColour(Slider::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    transposeSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    transposeSlider.setColour(Slider::textBoxTextColourId, Colours::gold.withBrightness(0.95f));
    sliderAttachments.add(new SliderAttachment(vts, "Transpose", transposeSlider));
    tab1.addAndMakeVisible(transposeSlider);
    
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
       if (i == 0) { //EBSPECIFIC
            pitchBendSliders.add(new Slider());
            pitchBendSliders[i]->setSliderStyle(Slider::SliderStyle::LinearBar);
            pitchBendSliders[i]->setInterceptsMouseClicks(false, false);
            pitchBendSliders[i]->setLookAndFeel(&laf);
            pitchBendSliders[i]->setColour(Slider::trackColourId, Colours::lightgrey);
            pitchBendSliders[i]->setColour(Slider::backgroundColourId, Colours::black);
            pitchBendSliders[i]->setColour(Slider::textBoxOutlineColourId, Colours::grey);
            //pitchBendSliders[i]->setTextValueSuffix("m2");
            pitchBendSliders[i]->addListener(this);
            tab1.addAndMakeVisible(pitchBendSliders[i]);
            
            sliderAttachments.add(new SliderAttachment(vts, "PitchBend" + String(i),
                                                       *pitchBendSliders[i]));
        }
    }
    
    rangeSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    rangeSlider.setInterceptsMouseClicks(true, true);
    rangeSlider.setLookAndFeel(&laf);
    rangeSlider.setColour(Slider::trackColourId, Colours::lightgrey);
    rangeSlider.setColour(Slider::backgroundColourId, Colours::black);
    rangeSlider.setColour(Slider::textBoxOutlineColourId, Colours::grey);
    //pitchBendSliders[i]->setTextValueSuffix("m2");
    rangeSlider.addListener(this);
    tab1.addAndMakeVisible(rangeSlider);
    
    sliderAttachments.add(new SliderAttachment(vts, "PitchBendRangeUp",
                                               rangeSlider));
    sliderAttachments.add(new SliderAttachment(vts, "PitchBendRangeDown",
                                               rangeSlider));
    rangeLabel.setText("RANGE", dontSendNotification);
    rangeLabel.setLookAndFeel(&laf);
    rangeLabel.setJustificationType(Justification::centredRight);
    tab1.addAndMakeVisible(rangeLabel);
   
    
    
//    mpeToggle.setButtonText("MPE");
//    mpeToggle.addListener(this);
//    tab1.addAndMakeVisible(mpeToggle);
    

    keyboard.setAvailableRange(21, 108);
    keyboard.setOctaveForMiddleC(4);
    //    tab1.addAndMakeVisible(&keyboard);
    
    tab1.addAndMakeVisible(&envsAndLFOs);
    
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscModules.add(new OscModule(*this, vts, *processor.oscs[i]));
        tab1.addAndMakeVisible(oscModules[i]);
    }
    
    noiseModule = std::make_unique<NoiseModule>(*this, vts, *processor.noise);
    tab1.addAndMakeVisible(noiseModule.get());
    
    for (int i = 0; i < NUM_FILT; ++i)
    {
        filterModules.add(new FilterModule(*this, vts, *processor.filt[i]));
        tab1.addAndMakeVisible(filterModules[i]);
    }
    
    
    seriesParallelComponent.setOutlineColour(Colours::darkgrey);
    tab1.addAndMakeVisible(seriesParallelComponent);
    
    seriesParallelSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    seriesParallelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 10, 10);
    seriesParallelComponent.addAndMakeVisible(seriesParallelSlider);
    sliderAttachments.add(new SliderAttachment(vts, "Filter Series-Parallel Mix",
                                               seriesParallelSlider));
    
    seriesLabel.setText("Ser.", dontSendNotification);
    seriesLabel.setLookAndFeel(&laf);
    seriesParallelComponent.addAndMakeVisible(seriesLabel);
    
    parallelLabel.setText("Par.", dontSendNotification);
    parallelLabel.setJustificationType(Justification::centredRight);
    parallelLabel.setLookAndFeel(&laf);
    seriesParallelComponent.addAndMakeVisible(parallelLabel);
    
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
    tab1.addAndMakeVisible(OSCILLOSCOPE);
    //==============================================================================
    // TAB2 ========================================================================
    addAndMakeVisible(tab2);
    
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        if (i < NUM_MACROS)
        {
            String n = "M" + String(i+1);
            if (i >= NUM_GENERIC_MACROS) n = cUniqueMacroNames[i-NUM_GENERIC_MACROS];
            macroControlLabels.add(new Label());
            macroControlLabels.getLast()->setText(n + " CC#", dontSendNotification);
            macroControlLabels.getLast()->setLookAndFeel(&laf);
            tab2.addAndMakeVisible(macroControlLabels.getLast());
        }
        
        macroControlEntries.add(new Label());
        macroControlEntries.getLast()->setLookAndFeel(&laf);
        macroControlEntries.getLast()->setEditable(true);
        macroControlEntries.getLast()->setJustificationType(Justification::centred);
        macroControlEntries.getLast()->setColour(Label::backgroundColourId,
                                                Colours::darkgrey.withBrightness(0.2f));
        macroControlEntries.getLast()->addListener(this);
        tab2.addAndMakeVisible(macroControlEntries.getLast());
        
        if (i < NUM_GENERIC_MACROS)
        {
            macroControlNameLabels.add(new Label());
            macroControlNameLabels.getLast()->setText("Name", dontSendNotification);
            macroControlNameLabels.getLast()->setLookAndFeel(&laf);
            tab2.addAndMakeVisible(macroControlNameLabels.getLast());
            
            macroControlNames.add(new Label());
            macroControlNames.getLast()->setLookAndFeel(&laf);
            macroControlNames.getLast()->setEditable(true);
            macroControlNames.getLast()->setJustificationType(Justification::centred);
            macroControlNames.getLast()->setColour(Label::backgroundColourId,
                                                   Colours::darkgrey.withBrightness(0.2f));
            macroControlNames.getLast()->addListener(this);
            tab2.addAndMakeVisible(macroControlNames.getLast());
        }
        
    }
   
    
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
    {
        String n = "String " + String(i);
        if (i == 0) n = "Global Pitch Bend & CCs";
        stringChannelLabels.add(new Label());
        stringChannelLabels.getLast()->setText(n + " Ch#", dontSendNotification);
//        stringChannelLabels.getLast()->setJustificationType(Justification::centredRight);
        stringChannelLabels.getLast()->setLookAndFeel(&laf);
        tab2.addAndMakeVisible(stringChannelLabels.getLast());
        
        stringChannelEntries.add(new Label());
        stringChannelEntries.getLast()->setLookAndFeel(&laf);
        stringChannelEntries.getLast()->setEditable(true);
        stringChannelEntries.getLast()->setJustificationType(Justification::centred);
        stringChannelEntries.getLast()->setColour(Label::backgroundColourId,
                                                 Colours::darkgrey.withBrightness(0.2f));
        stringChannelEntries.getLast()->addListener(this);
        tab2.addAndMakeVisible(stringChannelEntries.getLast());
    }
    
    //==============================================================================
    // TAB3 ========================================================================
    addAndMakeVisible(tab3);
    
    tab3.addAndMakeVisible(tuningTab);
    
    //==============================================================================
    
    //==============================================================================
    // TAB4 ========================================================================
    addAndMakeVisible(tab4);
    
    tab4.addAndMakeVisible(fxTab);
    for (auto mapModel : p.sourceMap)
    {
        MappingSource *s = new MappingSource(*this, *mapModel);
        allSources.add(s);
        tab4.addAndMakeVisible(s);
    }
    //==============================================================================
    
    tabs.addTab("Synth", Colours::black, &tab1, false);
    tabs.addTab("Control", Colours::black, &tab2, false);
    tabs.addTab("Tuning", Colours::black, &tab3, false);
    tabs.addTab("FX", Colours::black, &tab4, false);
    
    tabs.getTabbedButtonBar().getTabButton(0)->addListener(this);
    tabs.getTabbedButtonBar().getTabButton(1)->addListener(this);
    tabs.getTabbedButtonBar().getTabButton(2)->addListener(this);
    tabs.getTabbedButtonBar().getTabButton(3)->addListener(this);
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
    //processor.setMPEMode(true); //HACKYEB
    update();
    startTimerHz(30);
}



ElectroAudioProcessorEditor::~ElectroAudioProcessorEditor()
{
    //    masterDial->setLookAndFeel(nullptr);
    //    ampDial->setLookAndFeel(nullptr);
    //    for (int i = 0; i < NUM_MACROS; ++i)
    //    {
    //        ccDials[i]->setLookAndFeel(nullptr);
    //    }
    pitchBendSliders[0]->setLookAndFeel(nullptr); //EBSPECIFIC

    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
       
        stringActivityButtons[i]->setLookAndFeel(nullptr);
    }
    
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        macroControlNameLabels[i]->setLookAndFeel(nullptr);
        macroControlNames[i]->setLookAndFeel(nullptr);
    }
    
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        if (i < NUM_MACROS) macroControlLabels[i]->setLookAndFeel(nullptr);
        macroControlEntries[i]->setLookAndFeel(nullptr);
    }
    
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
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
    
    numVoicesLabel.setLookAndFeel(nullptr);
    numVoicesSlider.setLookAndFeel(nullptr);
    transposeLabel.setLookAndFeel(nullptr);
    transposeSlider.setLookAndFeel(nullptr);
    rangeSlider.setLookAndFeel(nullptr);
    sliderAttachments.clear();
    buttonAttachments.clear();
    fxTab.setLookAndFeel(nullptr);
}

//==============================================================================
void ElectroAudioProcessorEditor::paint (Graphics& g)
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

void ElectroAudioProcessorEditor::resized()
{

    int width = getWidth();
    int height = getHeight();
    
    float s = width / EDITOR_WIDTH;
    
    processor.editorScale = 1.05f;
    // Set OSCILLOSCOPE bounds

    
    //tabs.setBounds(getLocalBounds().expanded(1));
    tabs.setBoundsRelative(0,0,1,1.0);
    tabs.setTabBarDepth(30*s);
    
    height -= tabs.getTabBarDepth();
    
    
   
    //==============================================================================
    // TAB1 ========================================================================
    for (int i = 0; i < NUM_OSCS; ++i)
    {
        oscModules[i]->setBounds(-1, (120*s*i)-i-1, 540*s+1, 120*s);
    }


    envsAndLFOs.setBounds(-1, oscModules.getLast()->getBottom()-1, 540*s+1, 160*s);
    envsAndLFOs.setIndent(10*s);
    envsAndLFOs.setTabBarDepth(25*s);
    
    noiseModule->setBounds(oscModules[0]->getRight()-1, -1, 364*s, 121*s-2);
    
    //ettingsComponent.setBounds(noiseModule->getRight()-1, -1, 114*s, 100*s-1);
    
    seriesParallelComponent.setBounds(540*s-1, noiseModule->getBottom()-1, 364*s, 40*s);
    seriesLabel.setBounds(0, 0, 60*s, 22*s);
    parallelLabel.setBounds(302*s, 0, 60*s, 22*s);
    seriesParallelSlider.setBounds(0, 18*s, 363*s, 22*s);
    
    filterModules[0]->setBounds(540*s-1, seriesParallelComponent.getBottom()-1, 360*s+2, 124*s);
    for (int i = 1; i < NUM_FILT; ++i)
    {
        filterModules[i]->setBounds(540*s-1, filterModules[i-1]->getBottom()-1, 360*s+2, 124*s);
    }
    
    outputModule->setBounds(540*s-1, filterModules.getLast()->getBottom()-1, 360*s+2, 114*s);
    
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
    
    midiKeyComponent.setBounds(-1, y-2, 610, 33);//657
    midiKeySource->setBounds(5, 7, x-40, 22*s - 4);
    midiKeyMinLabel.setBounds(midiKeySource->getRight()+4, 7, 40, 22*s - 4);
    midiKeyRangeSlider.setBounds(midiKeyMinLabel.getRight(), 7, 420, 22*s - 4);
    midiKeyMaxLabel.setBounds(midiKeyRangeSlider.getRight(), 7, 40, 22*s - 4);


        
    int r = (10*align) % 12;
    int w = (10*align) / 12;
    y = height-35*s+2;//
    //mpeToggle.setBounds(6*s, y, x-w-5*s, 35*s); //EBSPECIFIC
    //pitchBendSliders[0]->setBounds(0, midiKeyComponent.getBottom()-1, x, 27*s);
    stringActivityButtons[0]->setBounds(0, y, w, 35*s);
    pitchBendSliders[0]->setBounds(0,
                                   midiKeyComponent.getBottom()-1,
                                   w + (r > 0 ? 1 : 0), 27*s); //EBSPECIFIC
    rangeLabel.setBounds(pitchBendSliders[0]->getRight(), midiKeyComponent.getBottom()-1,w + (r > 0 ? 1 : 0), 12*s);
        rangeSlider.setBounds(pitchBendSliders[0]->getRight(), stringActivityButtons[0]->getY() -17*s ,w + (r > 0 ? 1 : 0), 17*s);
        
        numVoicesLabel.setBounds(rangeSlider.getRight(), midiKeyComponent.getBottom()-1, w + (r > 0 ? 1 : 0), 12*s);
        numVoicesSlider.setBounds(rangeSlider.getRight(), stringActivityButtons[0]->getY() -17*s, w + (r > 0 ? 1 : 0), 17*s);
        transposeLabel.setBounds(numVoicesSlider.getRight(), midiKeyComponent.getBottom()-1, w + (r > 0 ? 1 : 0), 12*s);
        transposeSlider.setBounds(numVoicesSlider.getRight(), stringActivityButtons[0]->getY() -17*s, w + (r > 0 ? 1 : 0), 17*s);
    velocityComponent.setBounds(transposeSlider.getRight() + 42, midiKeyComponent.getBottom()-1, 102, 33);
    velocitySource->setBounds(5, 7, 91, 22 * s - 4);

    randomComponent.setBounds(velocityComponent.getRight() - 1, midiKeyComponent.getBottom()-1, x + 60, 33);
    randomSource->setBounds(5, 7, x+2, 22*s - 4);
    randomValueLabel.setBounds(randomSource->getRight()+4, 7, 40, 22*s - 4);
    for (int i = 1; i < NUM_CHANNELS; ++i)
    {
        stringActivityButtons[i]->setBounds(stringActivityButtons[i-1]->getRight(), y,
                                           w + (r-- > 0 ? 1 : 0), 35*s);
    }
    
    //    keyboard.setBoundsRelative(0.f, 0.86f, 1.0f, 0.14f);
    //    keyboard.setKeyWidth(width / 52.0f);
    OSCILLOSCOPE.setBoundsRelative(0.65,0.87,0.35, 0.13 );

    //==============================================================================
    // TAB2 ========================================================================
    
    x = 40;
    y = 40;
    int h = 30;
    int pad = 4;
    
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        int padx = (i/4) < 3 ? 0 : 2;
        macroControlLabels[i]->setBounds(x + 216*(i/4) + padx, y+(h+pad)*(i%4)*2, 100, h);
        macroControlEntries[i]->setBounds(macroControlLabels[i]->getRight(),
                                          macroControlLabels[i]->getY(), 100, h);
        macroControlNameLabels[i]->setBounds(x + 216*(i/4) + padx,
                                             y+(h+pad)+(h+pad)*(i%4)*2, 70, h);
        macroControlNames[i]->setBounds(macroControlNameLabels[i]->getRight(),
                                        macroControlNameLabels[i]->getY(), 130, h);
    }
    DBG(macroControlEntries[NUM_GENERIC_MACROS-1]->getRight());
    
    y = 300;
    for (int i = NUM_GENERIC_MACROS; i < NUM_MACROS; ++i)
    {
        int j = i - NUM_GENERIC_MACROS;
        macroControlLabels[i]->setBounds(x + 216*(j/2), y+(h+pad)+(h+pad)*(j%2), 100, h);
        macroControlEntries[i]->setBounds(macroControlLabels[i]->getRight(),
                                          macroControlLabels[i]->getY(), 100, h);
//        if (i == NUM_MACROS - 1)
//        {
//            macroControlLabels[i]->setBounds(x + 216*(j/2),
//                                             y+(h+pad)+(h+pad)*((j%2)+0.5f),
//                                             100, h);
//            macroControlEntries[NUM_MACROS]->setBounds(macroControlLabels[i]->getRight(),
//                                                       y+(h+pad)+(h+pad)*((j%2)+1), 100, h);
//        }
    }
    
    y = 430;
    stringChannelLabels[0]->setBounds(x, y, 300, h);
    stringChannelEntries[0]->setBounds(stringChannelLabels[0]->getRight(),
                                       stringChannelLabels[0]->getY(), 100, h);
    for (int i = 1; i < MAX_NUM_VOICES+1; ++i)
    {
        stringChannelLabels[i]->setBounds(x + 300*((i-1)/4),
                                          y+(h+pad)+(h+pad)*((i-1)%4), 150, h);
        stringChannelEntries[i]->setBounds(stringChannelLabels[i]->getRight(),
                                           stringChannelLabels[i]->getY(), 100, h);
    }
    DBG(stringChannelEntries.getLast()->getRight());
    
    //==============================================================================
    // TAB3 ========================================================================
    
    tuningTab.setBoundsRelative(0.05f, 0.08f, 0.9f, 0.84f);
    
    //==============================================================================
    
    versionLabel.setBounds(width*0.79f, 0, width * 0.05f, tabs.getTabBarDepth());
    versionLabel.setFont(euphemia.withHeight(20*s));
    
    sendOutButton.setBounds(width*0.85f, -1, width*0.15f+2, tabs.getTabBarDepth());
    
    int logoLeft = tabs.getTabbedButtonBar().getTabButton(2)->getRight() + 90*s;
    Rectangle<float> logoArea (logoLeft, 0, 98*s, tabs.getTabBarDepth());
    logo->setTransformToFit (logoArea,
                             RectanglePlacement::xLeft +
                             RectanglePlacement::yTop +
                             RectanglePlacement::fillDestination);
    synderphonicsLabel.setBounds(logoLeft+50*s, -5*s, 220*s, 34*s);
    synderphonicsLabel.setFont(euphemia.withHeight(34*s));
    ElectrobassLabel.setBounds(synderphonicsLabel.getRight(), -5*s, 300*s, 34*s);
    ElectrobassLabel.setFont(euphemia.withHeight(34*s).withStyle(3));
    
    float rt = EDITOR_WIDTH / EDITOR_HEIGHT;
    constrain->setSizeLimits(200, 200/rt, 800*rt, 800);
    resizer->setBounds(getWidth()-12, getHeight()-12, 12, 12);
    
    //    container.setBounds(getLocalBounds());
    
    //==============================================================================
    // TAB3 ========================================================================
    
    fxTab.setBoundsRelative(0.05f, 0.08f, 1.0f, 1.0f);
    for (int i = 0; i < allSources.size(); ++i)
    {
        allSources[i]->setBounds(6*s + (75+2)*(i/3), 523*s + ((i%3)*(40*0.5f)), 75, 40*0.5f);
    }
    //==============================================================================
  
}

void ElectroAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == nullptr) return;
    
    if (slider == &midiKeyRangeSlider)
    {
        updateMidiKeyRangeSlider(midiKeyRangeSlider.getMinValue(),
                                 midiKeyRangeSlider.getMaxValue());
    }
    else if (slider == &numVoicesSlider)
    {
        updateNumVoicesSlider(numVoicesSlider.getValue()); //EBSPECIFIC
    }
    
    if (slider == &rangeSlider)
    {
        vts.getParameter("PitchBendRangeUp")->setValue(rangeSlider.getValue());
        processor.pitchBendRange->end = rangeSlider.getValue();
        processor.pitchBendRange->setSkewForCentre(0.0f);
        vts.getParameter("PitchBendRangeDown")->setValue(rangeSlider.getValue());
        processor.pitchBendRange->start = - rangeSlider.getValue();
        processor.pitchBendRange->setSkewForCentre(0.0f);
        //pitchBendSliders[0]->setRange(- (rangeDownSlider.getValue()), rangeUpSlider.getValue());
    }
}

void ElectroAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == nullptr) return;

    if (ToggleButton* tb = dynamic_cast<ToggleButton*>(button))
    {
        if (tb == &mpeToggle)
        {
            updateMPEToggle(tb->getToggleState());
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
        tab2.addAndMakeVisible(mpeToggle);
        for (auto slider : pitchBendSliders) tab2.addAndMakeVisible(slider);
        for (auto button : stringActivityButtons) tab2.addAndMakeVisible(button);
    }
    else if (button == tabs.getTabbedButtonBar().getTabButton(2))
    {
    }
}

void ElectroAudioProcessorEditor::labelTextChanged(Label* label)
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
    else if (macroControlNames.contains(label))
    {
        String name = label->getText().substring(0, 8);
        updateMacroNames(macroControlNames.indexOf(label), name);
    }
    else if (stringChannelEntries.contains(label))
    {
        int ch = label->getText().getIntValue();
        updateStringChannel(stringChannelEntries.indexOf(label), ch);
    }
}

void ElectroAudioProcessorEditor::mouseDown (const MouseEvent &event)
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

bool ElectroAudioProcessorEditor::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    return false;
}

void ElectroAudioProcessorEditor::timerCallback()
{
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
    {
        stringActivityButtons[i]->setToggleState(processor.stringIsActive(i),
                                                 dontSendNotification);
    }
    updateRandomValueLabel(processor.lastRandomValue);

}

void ElectroAudioProcessorEditor::update()
{
    updateMPEToggle(processor.getMPEMode());
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
    {
        updateStringChannel(i, processor.stringChannels[i]);
    }
    for (int i = 0; i < NUM_MACROS+1; ++i)
    {
        updateMacroControl(i, processor.macroCCNumbers[i]);
    }
    for (int i = 0; i < NUM_GENERIC_MACROS; ++i)
    {
        updateMacroNames(i, processor.macroNames[i]);
    }
    updateMidiKeyRangeSlider(processor.midiKeyMin, processor.midiKeyMax);
    updateNumVoicesSlider(processor.numVoicesActive);
    updateRandomValueLabel(processor.lastRandomValue);
    for(auto osc : oscModules)
    {
        osc->updateShapeCB();
    }
}


void ElectroAudioProcessorEditor::updateMPEToggle(bool state)
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

void ElectroAudioProcessorEditor::updateStringChannel(int string, int ch)
{
    ch = jlimit(0, 16, ch);
    // Handle mapping that will be overwritten
    for (int i = 0; i < MAX_NUM_VOICES+1; ++i)
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
    bool state = processor.getMPEMode();
    String text = "All";
    if (state) text = String(processor.stringChannels[string]);
    stringChannelEntries[string]->setAlpha(state ? 1.f : 0.7f);
    stringChannelEntries[string]->setText(text, dontSendNotification);
    stringChannelEntries[string]->setEnabled(state);
}

void ElectroAudioProcessorEditor::updateMacroControl(int macro, int ctrl)
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

void ElectroAudioProcessorEditor::updateMacroNames(int macro, String name)
{
    processor.macroNames.set(macro, name);
    macroControlNames[macro]->setText(processor.macroNames[macro], dontSendNotification);
}

void ElectroAudioProcessorEditor::updateMidiKeyRangeSlider(int min, int max)
{
    processor.midiKeyMin = min;
    processor.midiKeyMax = max;
    midiKeyRangeSlider.setMinAndMaxValues(processor.midiKeyMin, processor.midiKeyMax,
                                          dontSendNotification);
    midiKeyMinLabel.setText(String(processor.midiKeyMin), dontSendNotification);
    midiKeyMaxLabel.setText(String(processor.midiKeyMax), dontSendNotification);
}

void ElectroAudioProcessorEditor::updateNumVoicesSlider(int numVoices)
{
    //processor.numVoicesActive = numVoices;
    processor.setNumVoicesActive(numVoices);
    numVoicesSlider.setValue(numVoices, dontSendNotification);
    for (int i = 0; i < MAX_NUM_VOICES; ++i)
    {
        stringActivityButtons[i+1]->setAlpha(i+1 > processor.numVoicesActive ? 0.5f : 1.f);
    }
    
}

void ElectroAudioProcessorEditor::updateRandomValueLabel(float value)
{
    randomValueLabel.setText(String(value, 3), dontSendNotification);
}
