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
valueTreeState(vts),
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
        
        sliderAttachments.add(new SliderAttachment(valueTreeState, "PitchBendCh" + String(i),
                                                   *pitchBendSliders[i]));
    }
    channelSelection[0]->setButtonText("1 (Global)");
    channelSelection[1]->setToggleState(true, sendNotification);
    
    keyboard.setAvailableRange(21, 108);
    keyboard.setOctaveForMiddleC(4);
    addAndMakeVisible(&keyboard);
    
    for (int i = 0; i < SubtractiveKnobParamNil; i++) {
        stDials.add(new Slider());
        stDials[i]->setLookAndFeel(&laf);
        stDials[i]->setSliderStyle(Slider::RotaryVerticalDrag);
        stDials[i]->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        stDials[i]->setRange(0., 1.);
        stDials[i]->addListener(this);
        addAndMakeVisible(stDials[i]);
        
        dialLabels.add(new Label());
        dialLabels[i]->setText(String(cSubtractiveKnobParamNames[i]).replace("Subtractive", ""),
                               dontSendNotification);
        //        dialLabels[i]->setMultiLine(true);
        //        dialLabels[i]->setReadOnly(true);
        dialLabels[i]->setFont(euphemia);
        //        dialLabels[i]->setInterceptsMouseClicks(false, false);
        dialLabels[i]->setJustificationType(Justification::centredTop);
        dialLabels[i]->setLookAndFeel(&laf);
        addAndMakeVisible(dialLabels[i]);
        
        sliderAttachments.add(new SliderAttachment(valueTreeState, cSubtractiveKnobParamNames[i], *stDials[i]));
    }
    
    Path path;
    path.addEllipse(0, 0, 30, 30);
    
    for (int i = 0; i < 0; i++)
    {
        buttons.add(new ESButton("", Colours::white, Colours::grey.brighter(), Colours::darkgrey));
        buttons[i]->setShape(path, true, true, true);
        addAndMakeVisible(buttons[i]);
        buttons[i]->addListener(this);
        buttons[i]->setOpaque(true);
    }
    
    for (int i = 0; i < 0; i++)
    {
        lights.add(new ESLight("", Colours::grey, Colours::red));
        addAndMakeVisible(lights[i]);
        lights[i]->setOpaque(true);
    }
    
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
    for (int i = 0; i < SubtractiveKnobParamNil; i++)
    {
        stDials[i]->setLookAndFeel(nullptr);
        dialLabels[i]->setLookAndFeel(nullptr);
    }
}

//==============================================================================
void ESAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(0,0), Colour(10, 10, 10), juce::Point<float>(0, getHeight()), false));
    
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
    processor.editorScale = s;
    
    //    screen.setBounds(width*0.347f, height*0.096f, width*0.306f, height*0.105f);
    
    const float buttonSize = 24.0f*s;
    const float knobSize = 57.0f*s;
    const float bigLightSize = 23.0f*s;
    const float smallLightSize = 15.0f*s;
    const float labelWidth = 130.0f*s;
    const float labelHeight = 20.0f*s;
    
    //    buttons[vocodec::ButtonA]       ->setBounds(543*s, 356*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonB]       ->setBounds(543*s, 415*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonC]       ->setBounds(303*s, 526*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonD]       ->setBounds(184*s, 621*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonE]       ->setBounds(241*s, 621*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonEdit]    ->setBounds(422*s, 91*s,  buttonSize, buttonSize);
    //    buttons[vocodec::ButtonLeft]    ->setBounds(441*s, 145*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonRight]   ->setBounds(531*s, 145*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonUp]      ->setBounds(485*s, 121*s, buttonSize, buttonSize);
    //    buttons[vocodec::ButtonDown]    ->setBounds(485*s, 177*s, buttonSize, buttonSize);
    //
    //    lights[vocodec::ESLightUSB]         ->setBounds(493*s, 252*s, bigLightSize);
    //    lights[vocodec::ESLight1]           ->setBounds(502*s, 296*s, bigLightSize);
    //    lights[vocodec::ESLight2]           ->setBounds(539*s, 296*s, bigLightSize);
    //    lights[vocodec::ESLightA]           ->setBounds(510*s, 356*s, bigLightSize);
    //    lights[vocodec::ESLightB]           ->setBounds(510*s, 415*s, bigLightSize);
    //    lights[vocodec::ESLightC]           ->setBounds(303*s, 493*s, bigLightSize);
    //    lights[vocodec::ESLightEdit]        ->setBounds(422*s, 50*s,  bigLightSize);
    //    lights[vocodec::ESLightIn1Meter]    ->setBounds(25*s,  398*s, smallLightSize);
    //    lights[vocodec::ESLightIn1Clip]     ->setBounds(45*s,  398*s, smallLightSize);
    //    lights[vocodec::ESLightIn2Meter]    ->setBounds(25*s,  530*s, smallLightSize);
    //    lights[vocodec::ESLightIn2Clip]     ->setBounds(45*s,  530*s, smallLightSize);
    //    lights[vocodec::ESLightOut1Meter]   ->setBounds(538*s, 620*s, smallLightSize);
    //    lights[vocodec::ESLightOut1Clip]    ->setBounds(558*s, 620*s, smallLightSize);
    //    lights[vocodec::ESLightOut2Meter]   ->setBounds(538*s, 503*s, smallLightSize);
    //    lights[vocodec::ESLightOut2Clip]    ->setBounds(558*s, 503*s, smallLightSize);
    //
    
    stDials[SubtractiveVolume]->setBounds(50*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveShape]->setBounds(100*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveDetune]->setBounds(150*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveAttack]->setBounds(200*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveDecay]->setBounds(250*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveSustain]->setBounds(300*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveRelease]->setBounds(350*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveLeak]->setBounds(400*s, 50*s, knobSize, knobSize);
    stDials[SubtractiveFilterAmount]->setBounds(50*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterCutoff]->setBounds(100*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterKeyFollow]->setBounds(150*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterQ]->setBounds(200*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterAttack]->setBounds(250*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterDecay]->setBounds(300*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterSustain]->setBounds(350*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterRelease]->setBounds(400*s, 150*s, knobSize, knobSize);
    stDials[SubtractiveFilterLeak]->setBounds(450*s, 150*s, knobSize, knobSize);
    
    for (int i = 0; i < SubtractiveKnobParamNil; i++)
    {
        int c = i % 8;
        int r = i / 8;
        stDials[i]->setBounds(c*90*s + 45*s, r*100*s + 100*s, knobSize, knobSize);
        
        dialLabels[i]->setBounds(stDials[i]->getX() - knobSize*0.5f, stDials[i]->getY() + knobSize*1.1f,
                                 labelWidth, labelHeight);
    }
    //    dialLabels[0]                   ->setBounds(40*s, 110*s, 100*s, 50*s);
    //    dialLabels[1]                   ->setBounds(138*s, 270*s, labelWidth, labelHeight);
    //    dialLabels[2]                   ->setBounds(348*s, 270*s, labelWidth, labelHeight);
    //    dialLabels[3]                   ->setBounds(213*s, 410*s, labelWidth, labelHeight);
    //    dialLabels[4]                   ->setBounds(373*s, 410*s, labelWidth, labelHeight);
    //    dialLabels[5]                   ->setBounds(138*s, 570*s, labelWidth, labelHeight);
    //    dialLabels[6]                   ->setBounds(343*s, 570*s, labelWidth, labelHeight);
    
    for (auto label : dialLabels)
        label->setFont(euphemia.withHeight(height * 0.027f));
    
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
    
    TextButton* tb = dynamic_cast<TextButton*>(button);
    if (tb != nullptr)
    {
        int channel = channelSelection.indexOf(tb) + 1;
        keyboard.setMidiChannel(channel);
        keyboard.setAlpha(channel > 1 ? 1.0f : 0.5f);
        keyboard.setInterceptsMouseClicks(channel > 1, channel > 1);
        
        for (int i = 0; i < NUM_CHANNELS; ++i)
        {
            pitchBendSliders[i]->setVisible(false);
            if (i == channel) pitchBendSliders[i]->setVisible(true);
        }
    }
}

void ESAudioProcessorEditor::buttonStateChanged(Button* button)
{
    if (button == nullptr) return;
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
        //        int idx = processor.vcd.wavetableSynthParams.loadIndex;
        
        auto result = chooser.getResult();
        
        auto* reader = processor.formatManager.createReaderFor (result);
        
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource
                                                                      (reader, true));
            
            AudioBuffer<float> buffer = AudioBuffer<float>(reader->numChannels, int(reader->lengthInSamples));
            
            reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);
            
            //                if (processor.vcd.loadedTableSizes[idx] > 0)
            //                {
            //                    mpool_free((char*)processor.vcd.loadedTables[idx], processor.vcd.largePool);
            //                }
            //                processor.vcd.loadedTables[idx] =
            //                (float*) mpool_alloc(sizeof(float) * buffer.getNumSamples(), processor.vcd.largePool);
            //                processor.vcd.loadedTableSizes[idx] = buffer.getNumSamples();
            //                for (int i = 0; i < processor.vcd.loadedTableSizes[idx]; ++i)
            //                {
            //                    processor.vcd.loadedTables[idx][i] = buffer.getSample(0, i);
            //                }
            
            processor.readerSource.reset(newSource.release());
            
            //            processor.wavetablePaths.set(idx, result.getFullPathName());
        }
    });
}


