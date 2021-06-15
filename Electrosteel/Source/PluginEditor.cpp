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
        macroDials.add(new ESDial(*this, "M" + String(i+1), "M" + String(i+1), Colours::red,
                                  processor.ccParams[i]->getValuePointer(), 1, false));
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
    
    mpeToggle.setButtonText("MPE");
    mpeToggle.setToggleState(processor.getMPEMode(), dontSendNotification);
    mpeToggle.addListener(this);
    tab1.addAndMakeVisible(mpeToggle);
    
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
        ->setExtraComponent(new MappingSource(*this, paramName, displayName,
                                              processor.envs[i]->getValuePointer(),
                                              NUM_STRINGS, false, Colours::cyan),
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
        ->setExtraComponent(new MappingSource(*this, paramName, displayName,
                                              processor.lfos[i]->getValuePointer(),
                                              NUM_STRINGS, true, Colours::lime),
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
    
    constrain->setFixedAspectRatio(EDITOR_WIDTH / EDITOR_HEIGHT);
    
//    addAndMakeVisible(*resizer);
    resizer->setAlwaysOnTop(true);
    
    versionLabel.setJustificationType(Justification::centred);
    versionLabel.setBorderSize(BorderSize<int>(2));
    versionLabel.setText("v" + String(ProjectInfo::versionString), dontSendNotification);
    versionLabel.setColour(Label::ColourIds::textColourId, Colours::lightgrey);
    addAndMakeVisible(versionLabel);
    
    //    addAndMakeVisible(&container);
    
    for (auto target : targetMap)
    {
        target->updateMapping(sourceMap);
    }
    
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

void ESAudioProcessorEditor::addMappingSource(String name, MappingSource* source)
{
    sourceMap.set(name, source);
}

void ESAudioProcessorEditor::addMappingTarget(String name, MappingTarget* target)
{
    targetMap.set(name, target);
}

//==============================================================================
//==============================================================================

ESModule::ESModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts, AudioComponent& ac,
                   float relLeftMargin, float relDialWidth, float relDialSpacing,
                   float relTopMargin, float relDialHeight) :
editor(editor),
vts(vts),
ac(ac),
relLeftMargin(relLeftMargin),
relDialWidth(relDialWidth),
relDialSpacing(relDialSpacing),
relTopMargin(relTopMargin),
relDialHeight(relDialHeight),
outlineColour(Colours::transparentBlack)
{
    setInterceptsMouseClicks(false, true);
    
    for (int i = 0; i < ac.paramNames.size(); i++)
    {
        String paramName = ac.name + ac.paramNames[i];
        String displayName = ac.paramNames[i];
        dials.add(new ESDial(editor, paramName, displayName, ac.getParameterArray(i)));
        addAndMakeVisible(dials[i]);
        sliderAttachments.add(new SliderAttachment(vts, ac.name + ac.paramNames[i], dials[i]->getSlider()));
        dials[i]->getSlider().addListener(this);
        for (auto t : dials[i]->getTargets())
        {
            t->addListener(this);
            t->addMouseListener(this, true);
        }
    }
    
    if (ac.toggleable)
    {
        addAndMakeVisible(enabledToggle);
        buttonAttachments.add(new ButtonAttachment(vts, ac.name, enabledToggle));
    }
}

ESModule::~ESModule()
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboBoxAttachments.clear();
}

void ESModule::paint(Graphics &g)
{
    Rectangle<int> area = getLocalBounds();
    
    g.setColour(outlineColour);
    g.drawRect(area);
}

void ESModule::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    float h = area.getHeight();
    
    for (int i = 0; i < ac.paramNames.size(); ++i)
    {
        dials[i]->setBoundsRelative(relLeftMargin + (relDialWidth+relDialSpacing)*i, relTopMargin,
                            relDialWidth, relDialHeight);
    }
    
    if (ac.toggleable)
    {
        enabledToggle.setBounds(0, 0, h*0.2f, h*0.2f);
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
ESModule(editor, vts, ac, 0.05f, 0.132f, 0.05f, 0.18f, 0.78f)
{
    outlineColour = Colours::darkgrey;
    
    // Pitch slider should snap to ints
    getDial(OscPitch)->setRange(-24., 24., 1.);
    
    double pitch = getDial(OscPitch)->getSlider().getValue();
    double fine = getDial(OscFine)->getSlider().getValue()*0.01; // Fine is in cents for better precision
    pitchLabel.setText(String(pitch+fine, 3), dontSendNotification);
    pitchLabel.setLookAndFeel(&laf);
    pitchLabel.setEditable(true);
    pitchLabel.setJustificationType(Justification::centred);
    pitchLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    pitchLabel.addListener(this);
    addAndMakeVisible(pitchLabel);
    
    RangedAudioParameter* set = vts.getParameter(ac.name + "ShapeSet");
    shapeCB.addItemList(oscSetNames, 1);
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()));
    shapeCB.setLookAndFeel(&laf);
    addAndMakeVisible(shapeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.name + "ShapeSet", shapeCB));
    
    sendSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    sendSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);
    addAndMakeVisible(sendSlider);
    sliderAttachments.add(new SliderAttachment(vts, ac.name + "FilterSend", sendSlider));
    
    f1Label.setText("F1", dontSendNotification);
    f1Label.setJustificationType(Justification::bottomRight);
    f1Label.setLookAndFeel(&laf);
    addAndMakeVisible(f1Label);
    
    f2Label.setText("F2", dontSendNotification);
    f2Label.setJustificationType(Justification::topRight);
    f2Label.setLookAndFeel(&laf);
    addAndMakeVisible(f2Label);
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
    
    for (int i = 1; i < ac.paramNames.size(); ++i)
    {
        dials[i]->setBoundsRelative(relLeftMargin + (relDialWidth*(i+1))+(relDialSpacing*i),
                                    relTopMargin, relDialWidth, relDialHeight);
    }
    
    pitchLabel.setBoundsRelative(relLeftMargin+relDialWidth+0.5f*relDialSpacing,
                                 0.4f, relDialWidth, 0.2f);
    
    shapeCB.setBoundsRelative(relLeftMargin+3*relDialWidth+relDialSpacing, 0.01f,
                              relDialWidth+2*relDialSpacing, 0.16f);
    
    sendSlider.setBoundsRelative(0.94f, 0.f, 0.06f, 1.0f);
    
    f1Label.setBoundsRelative(0.9f, 0.05f, 0.06f, 0.15f);
    f2Label.setBoundsRelative(0.9f, 0.80f, 0.06f, 0.15f);
}

void OscModule::sliderValueChanged(Slider* slider)
{
    if (slider == &getDial(OscPitch)->getSlider() || slider == &getDial(OscFine)->getSlider() )
    {
        displayPitch();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
        displayPitchMapping(mt);
    }
}

void OscModule::labelTextChanged(Label* label)
{
    if (label == &pitchLabel)
    {
        auto value = pitchLabel.getText().getDoubleValue();
        int i = value;
        double f = value-i;
        getDial(OscPitch)->getSlider().setValue(i);
        getDial(OscFine)->getSlider().setValue(f*100.);
    }
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

void OscModule::displayPitch()
{
    auto pitch = getDial(OscPitch)->getSlider().getValue();
    auto fine = getDial(OscFine)->getSlider().getValue()*0.01;
    pitchLabel.setColour(Label::textColourId, Colours::gold.withBrightness(0.95f));
    pitchLabel.setText(String(pitch+fine, 3), dontSendNotification);
}

void OscModule::displayPitchMapping(MappingTarget* mt)
{
    if (!mt->isActive())
    {
        displayPitch();
        return;
    }
    auto value = fabs(mt->getValue());
    if (mt->getParentComponent() == getDial(OscPitch))
    {
        pitchLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar()) text = String::charToString(0xb1);
        else text = (value >= 0 ? "+" : "-");
        text += String(value, 3);
        pitchLabel.setText(text, dontSendNotification);
    }
    else if (mt->getParentComponent() == getDial(OscFine))
    {
        pitchLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar()) text = String::charToString(0xb1);
        else text = (value >= 0 ? "+" : "-");
        text += String(value*0.01, 3);
        pitchLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

FilterModule::FilterModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                     AudioComponent& ac) :
ESModule(editor, vts, ac, 0.05f, 0.2f, 0.05f, 0.2f, 0.7f)
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
    
    RangedAudioParameter* set = vts.getParameter(ac.name + "Type");
    typeCB.addItemList(filterTypeNames, 1);
    typeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()));
    typeCB.setLookAndFeel(&laf);
    addAndMakeVisible(typeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.name + "Type", typeCB));
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
    
    for (int i = 1; i < ac.paramNames.size(); ++i)
    {
        dials[i]->setBoundsRelative(relLeftMargin + (relDialWidth*(i+1))+(relDialSpacing*i),
                                    relTopMargin, relDialWidth, relDialHeight);
    }
    
    cutoffLabel.setBoundsRelative(relLeftMargin+relDialWidth+0.5f*relDialSpacing,
                                 0.42f, relDialWidth, 0.16f);
    
    typeCB.setBoundsRelative(relLeftMargin+relDialWidth, 0.01f,
                             relDialWidth+relDialSpacing, 0.16f);
}

void FilterModule::sliderValueChanged(Slider* slider)
{
    if (slider == &getDial(FilterCutoff)->getSlider())
    {
        displayCutoff();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
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
        auto value = fabs(mt->getValue());
        cutoffLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar()) text = String::charToString(0xb1);
        else text = (value >= 0 ? "+" : "-");
        text += String(value, 2);
        cutoffLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

EnvModule::EnvModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ESModule(editor, vts, ac, 0.04f, 0.13f, 0.0675f, 0.16f, 0.84f)
{
    velocityToggle.setButtonText("Scale to velocity");
    addAndMakeVisible(velocityToggle);
    buttonAttachments.add(new ButtonAttachment(vts, ac.name + "Velocity", velocityToggle));
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
ESModule(editor, vts, ac, 0.12f, 0.13f, 0.185f, 0.16f, 0.84f)
{
    double rate = getDial(LowFreqRate)->getSlider().getValue();
    rateLabel.setText(String(rate, 2) + " Hz", dontSendNotification);
    rateLabel.setLookAndFeel(&laf);
    rateLabel.setEditable(true);
    rateLabel.setJustificationType(Justification::centred);
    rateLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withBrightness(0.2f));
    rateLabel.addListener(this);
    addAndMakeVisible(rateLabel);
    
    RangedAudioParameter* set = vts.getParameter(ac.name + "ShapeSet");
    shapeCB.addItemList(oscSetNames, 1);
    shapeCB.setSelectedItemIndex(set->convertFrom0to1(set->getValue()));
    shapeCB.setLookAndFeel(&laf);
    addAndMakeVisible(shapeCB);
    comboBoxAttachments.add(new ComboBoxAttachment(vts, ac.name + "ShapeSet", shapeCB));
    
    syncToggle.setButtonText("Sync to note-on");
    addAndMakeVisible(syncToggle);
    buttonAttachments.add(new ButtonAttachment(vts, ac.name + "Sync", syncToggle));
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
    if (slider == &getDial(LowFreqRate)->getSlider())
    {
        displayRate();
    }
    else if (MappingTarget* mt = dynamic_cast<MappingTarget*>(slider))
    {
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
    rateLabel.setText(String(rate, 2) + " Hz", dontSendNotification);
}

void LFOModule::displayRateMapping(MappingTarget* mt)
{
    if (!mt->isActive()) displayRate();
    else if (mt->getParentComponent() == getDial(LowFreqRate))
    {
        auto value = fabs(mt->getValue());
        rateLabel.setColour(Label::textColourId, mt->getColour());
        String text;
        if (mt->isBipolar()) text = String::charToString(0xb1);
        else text = (value >= 0 ? "+" : "");
        text += String(value, 2) + " Hz";
        rateLabel.setText(text, dontSendNotification);
    }
}

//==============================================================================
//==============================================================================

OutputModule::OutputModule(ESAudioProcessorEditor& editor, AudioProcessorValueTreeState& vts,
                           AudioComponent& ac) :
ESModule(editor, vts, ac, 0.1f, 0.2f, 0.1f, 0.125f, 0.75f)
{
    outlineColour = Colours::darkgrey;
    
    masterDial = std::make_unique<ESDial>(editor, "Master");
//    sliderAttachments.add(new SliderAttachment(vts, "Master", masterDial->getSlider()));
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

