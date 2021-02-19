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

ESAudioProcessorEditor::ESAudioProcessorEditor (ESAudioProcessor& p) :
AudioProcessorEditor (&p), processor (p),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain.get())),
chooser("Select a .wav file to load...", {}, "*.wav")
{
    panel = Drawable::createFromImageData(BinaryData::panel_svg, BinaryData::panel_svgSize);
    
    setWantsKeyboardFocus(true);
    
    Typeface::Ptr tp = Typeface::createSystemTypefaceFor(BinaryData::EuphemiaCAS_ttf,
                                                         BinaryData::EuphemiaCAS_ttfSize);
    euphemia = Font(tp);
    euphemia.setItalic(true);
    
    for (int i = 0; i < 0; i++) {
        //        knobs.add(new DrawableImage());
        dials.add(new Slider());
        addAndMakeVisible(dials[i]);
        //        addAndMakeVisible(knobs[i]);
        dials[i]->setLookAndFeel(&laf);
        dials[i]->setSliderStyle(Slider::RotaryVerticalDrag);
        dials[i]->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        dials[i]->addListener(this);
        dials[i]->setRange(0., 1.);
        dials[i]->setOpaque(true);
        
        dialLabels.add(new Label());
        //        dialLabels[i]->setMultiLine(true);
        //        dialLabels[i]->setReadOnly(true);
        dialLabels[i]->setFont(euphemia);
        //        dialLabels[i]->setInterceptsMouseClicks(false, false);
        dialLabels[i]->setJustificationType(Justification::centredTop);
        //        dialLabels[i]->setBorder(BorderSize<int>(-3, 0, 0, 0));
        dialLabels[i]->setLookAndFeel(&laf);
        addAndMakeVisible(dialLabels[i]);
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
    
    versionLabel.setText("v" + String(ProjectInfo::versionString), dontSendNotification);
    versionLabel.setColour(Label::ColourIds::textColourId, Colours::lightgrey);
    addAndMakeVisible(versionLabel);
    
    startTimerHz(30);
}

ESAudioProcessorEditor::~ESAudioProcessorEditor()
{
    for (int i = 0; i < 0; i++)
    {
        dials[i]->setLookAndFeel(nullptr);
        dialLabels[i]->setLookAndFeel(nullptr);
    }
}

//==============================================================================
void ESAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setGradientFill(ColourGradient(Colour(25, 25, 25), juce::Point<float>(0,0), Colour(10, 10, 10), juce::Point<float>(0, getHeight()), false));
    
    g.fillRect(0, 0, getWidth(), getHeight());
    
    Rectangle<float> panelArea = getLocalBounds().toFloat();
    panelArea.reduce(getWidth()*0.025f, getHeight()*0.01f);
    panelArea.removeFromBottom(getHeight()*0.03f);
    panel->drawWithin(g, panelArea, RectanglePlacement::centred, 1.0f);
    
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
    //    dials[0]                        ->setBounds(85*s, 60*s, knobSize, knobSize);
    //    dials[1]                        ->setBounds(175*s, 205*s, knobSize, knobSize);
    //    dials[2]                        ->setBounds(385*s, 205*s, knobSize, knobSize);
    //    dials[3]                        ->setBounds(250*s, 345*s, knobSize, knobSize);
    //    dials[4]                        ->setBounds(445*s, 345*s, knobSize, knobSize);
    //    dials[5]                        ->setBounds(175*s, 500*s, knobSize, knobSize);
    //    dials[6]                        ->setBounds(380*s, 500*s, knobSize, knobSize);
    //
    //    dialLabels[0]                   ->setBounds(40*s, 110*s, 100*s, 50*s);
    //    dialLabels[1]                   ->setBounds(138*s, 270*s, labelWidth, labelHeight);
    //    dialLabels[2]                   ->setBounds(348*s, 270*s, labelWidth, labelHeight);
    //    dialLabels[3]                   ->setBounds(213*s, 410*s, labelWidth, labelHeight);
    //    dialLabels[4]                   ->setBounds(373*s, 410*s, labelWidth, labelHeight);
    //    dialLabels[5]                   ->setBounds(138*s, 570*s, labelWidth, labelHeight);
    //    dialLabels[6]                   ->setBounds(343*s, 570*s, labelWidth, labelHeight);
    
    for (auto label : dialLabels)
        label->setFont(euphemia.withHeight(height * 0.027f));
    
    versionLabel.setBounds(0, height * 0.97f, width * 0.2f, height * 0.03f);
    versionLabel.setFont(euphemia.withHeight(height * 0.025f));
    
    float r = EDITOR_WIDTH / EDITOR_HEIGHT;
    constrain->setSizeLimits(200, 200/r, 800*r, 800);
    resizer->setBounds(getWidth()-16, getHeight()-16, 16, 16);
}

void ESAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == nullptr) return;
    
    
}

void ESAudioProcessorEditor::buttonClicked(Button*button)
{
}

void ESAudioProcessorEditor::buttonStateChanged(Button *button)
{
    if (button == nullptr) return;
    
    
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

