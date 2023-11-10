/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include "Electro_backend/FXTab.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Electro_backend/ElectroModules.h"
#include "Electro_backend/ElectroComponents.h"
#include "Constants.h"


//==============================================================================
/**
*/
//==============================================================================
/**
 An effect filter that simple makes image monochrome.
 (This will only work on images/components that aren't opaque, of course).
 @see Component::setComponentEffect
 */
class MonochromeEffect  : public ImageEffectFilter
{
public:
    //==============================================================================
    /** Creates a default drop-shadow effect.
     To customise the shadow's appearance, use the setShadowProperties() method.
     @param backgroundColour (optional) - for some alpha values you might prefer a background (such as black).
     */
    MonochromeEffect(juce::Colour backgroundColour = juce::Colours::transparentBlack) { bgColour_ = backgroundColour; }

    /** Destructor. */
    ~MonochromeEffect() {}

    //==============================================================================
    /** @internal */
    void applyEffect (juce::Image& image, juce::Graphics& g, float scaleFactor, float alpha)
    {
        g.fillAll(bgColour_);
        image.desaturate();
        g.setOpacity (alpha);
        g.drawImageAt (image, 0, 0);
    }
private:
    //==============================================================================
    juce::Colour bgColour_;
    JUCE_LEAK_DETECTOR (MonochromeEffect)
};
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class ElectroAudioProcessorEditor : public AudioProcessorEditor,
                               public Slider::Listener,
                               public Button::Listener,
                               public Label::Listener,
                               public KeyListener,
                               public Timer,
                               public DragAndDropContainer,
                               public sd::SoundMeter::MetersComponent::FadersChangeListener,
                               public ValueTree::Listener
                              

{
public:
    ElectroAudioProcessorEditor (ElectroAudioProcessor&, AudioProcessorValueTreeState& vts);
	~ElectroAudioProcessorEditor();
	
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(juce::Button* button) override;
    void labelTextChanged(Label* label) override;
    
    void mouseDown (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    void timerCallback() override;
    //void currentTabChanged(int tabIndex, const String &newCurrentTabName) override;
    void update();
    Slider masterSlider;
    ElectroAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;
    void  valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    void updateMPEToggle(bool state);
    void updatePedalVolumeControl(bool state);
    void updateNumVoicesSlider(int numVoices);
private:
    LookAndFeel_V4 laf;
    // Updating things that don't have attachments to the vts
    
    void updateStringChannel(int string, int ch);
    void updateMacroControl(int macro, int ctrl);
    void updateMacroNames(int macro, String name);
    void updateMidiKeyRangeSlider(int min, int max);
    void updateFXOrder(bool order)
    {
        
    }
    void updateVelocityLabel(float velocity);
    void updateRandomValueLabel(float value);
    static void setVerticalRotatedWithBounds (Component& component, bool clockWiseRotation, Rectangle<int> verticalBounds)
    {
        auto angle = MathConstants<float>::pi / 2.0f;

        if (! clockWiseRotation)
            angle *= -1.0f;

        component.setTransform ({});
        component.setSize (verticalBounds.getHeight(), verticalBounds.getWidth());
        component.setCentrePosition (0, 0);
        component.setTransform (AffineTransform::rotation (angle).translated (verticalBounds.getCentreX(), verticalBounds.getCentreY()));
    }
    
    void fadersChanged (std::vector<float> faderValues) override;
    
    
    File getLastFile() const
    {
        File f;

        f = File (settings.getValue ("lastStateFile"));

        if (f == File())
            f = File::getSpecialLocation (File::userDocumentsDirectory);

        return f;
    }

    void setLastFile (const FileChooser& fc)
    {
            settings.setValue ("lastStateFile", fc.getResult().getFullPathName());
    }
    sd::SoundMeter::MetersComponent meters;
    
    TextEditor presetNameEditor;
    Slider presetNumber;
    Label presetNamelabel;
    Label presetNumberlabel;
    
    WaveformComponent<float> OSCILLOSCOPE;
    
    TabbedComponent tabs;
    
    Component tab1;
    
    ElectroComponent midiKeyComponent;
    std::unique_ptr<MappingSource> midiKeySource;
    Slider midiKeyRangeSlider;
    Label midiKeyMinLabel;
    Label midiKeyMaxLabel;
    ElectroComponent velocityComponent;
    std::unique_ptr<MappingSource> velocitySource;
    ElectroComponent randomComponent;
    std::unique_ptr<MappingSource> randomSource;
    Label randomValueLabel;
    ElectroComponent uniqueMacroComponent;
    OwnedArray<ElectroDial> macroDials;
    OwnedArray<Slider> pitchBendSliders;
    MidiKeyboardComponent keyboard;
    OwnedArray<Button> stringActivityButtons;
    OwnedArray<OscModule> oscModules;
    std::unique_ptr<NoiseModule> noiseModule;
    OwnedArray<FilterModule> filterModules;
    std::unique_ptr<OutputModule> outputModule;
    ElectroTabbedComponent envsAndLFOs;
    MappingSource* currentMappingSource;
    ToggleButton mpeToggle;
    ToggleButton muteToggle;
    ToggleButton pedalControlsMasterToggle;
    ElectroComponent seriesParallelComponent;
    Slider seriesParallelSlider;
    Label seriesLabel;
    Label parallelLabel;
    
    Label numVoicesLabel;
    Slider numVoicesSlider;
    Label transposeLabel;
    Slider transposeSlider;
    
    Slider rangeSlider;
    
   
    
    
    Label rangeLabel;
    Component tab3;
    TuningTab tuningTab;
    Component tab2;
    Component tab4;
    Component tab5;
    CopedentTable copedentTable;
    
    FXTab fxTab;
    /* ToggleButton mpeToggle */// Declared above but will be include in this tab too
    OwnedArray<Label> macroControlEntries;
    OwnedArray<Label> macroControlNames;
    OwnedArray<Label> macroControlLabels;
    OwnedArray<Label> macroControlNameLabels;
    OwnedArray<Label> abxyControlNames;
    OwnedArray<Label> abxyControlLabels;
    OwnedArray<Label> abxyControlNameLabels;
    OwnedArray<Label> stringChannelEntries;
    OwnedArray<Label> stringChannelLabels;
    
    TextButton sendOutButton;
    ToggleButton streamChangesButton;
    Label versionLabel;
    std::unique_ptr<Drawable> logo;
    Label synderphonicsLabel;
    Label ElectrobassLabel;
    TextButton saveStateButton;
    TextButton loadStateButton;
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    ToggleButton stopProcessingToggle;
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    std::unique_ptr<ResizableCornerComponent> resizer;
    juce::TooltipWindow tooltipWindow;
    std::unique_ptr<FileChooser> stateFileChooser;
    Font euphemia;
    FileChooser chooser;
    OwnedArray<MappingSource> allSources;
    std::unique_ptr<Drawable> white_circle_image;
    std::unique_ptr<Drawable> hamburger_menu_image;
    //==============================================================================
    /*
    */
    
    class PopupMenuLAF : public LookAndFeel_V4, public DeletedAtShutdown
    {
    public:
        PopupMenuLAF()
        {
            setColour(PopupMenu::backgroundColourId, Colours::black);
            setColour(PopupMenu::textColourId, Colours::antiquewhite);
        }
        
        void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                                const bool isSeparator, const bool isActive,
                                                const bool isHighlighted, const bool isTicked,
                                                const bool hasSubMenu, const String& text,
                                                const String& shortcutKeyText,
                                                const Drawable* icon, const Colour* const textColourToUse)
        {
            if (isSeparator)
            {
                auto r  = area.reduced (5, 0);
                r.removeFromTop (roundToInt ((r.getHeight() * 0.5f) - 0.5f));
                
                g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
                g.fillRect (r.removeFromTop (1));
            }
            else
            {
                auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                   : *textColourToUse);
                
                auto r  = area.reduced (1);
                
                if (isHighlighted && isActive)
                {
                    g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
                    g.fillRect (r);
                    
                    g.setColour (findColour (PopupMenu::highlightedTextColourId));
                }
                else
                {
                    g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
                }
                
                r.reduce (jmin (5, area.getWidth() / 20), 0);
                
                auto font = getPopupMenuFont();
                
                const auto maxFontHeight = r.getHeight() / 1.3f;
                
                if (font.getHeight() > maxFontHeight)
                    font.setHeight (maxFontHeight);
                
                g.setFont (font);
                
                auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();
                
                if (icon != nullptr)
                {
                    icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
                }
                else if (isTicked)
                {
                    const auto tick = getTickShape (1.0f);
                    g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
                }
                
                if (hasSubMenu)
                {
                    const auto arrowH = 0.6f * getPopupMenuFont().getAscent();
                    
                    const auto x = (float) r.removeFromRight ((int) arrowH).getX();
                    const auto halfH = (float) r.getCentreY();
                    
                    Path path;
                    path.startNewSubPath (x, halfH - arrowH * 0.5f);
                    path.lineTo (x + arrowH * 0.6f, halfH);
                    path.lineTo (x, halfH + arrowH * 0.5f);
                    
                    g.strokePath (path, PathStrokeType (2.0f));
                }
                
                r.removeFromRight (3);
               
                g.drawFittedText (text, r, Justification::centredLeft, 1);
                
                
            }
        }
        Font getPopupMenuFont (void)
        {
            return Font (17.0f);
        }
    };
    class EBassPopupMenu : public PopupMenu
    {
    public:
        EBassPopupMenu()
        {
            setLookAndFeel(new PopupMenuLAF());
        }
        
        ~EBassPopupMenu()
        {
            setLookAndFeel(nullptr);
        }
        
    private:
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EBassPopupMenu)
    };
    
    PopupMenu getHamburgerMenu(void)
    {
        EBassPopupMenu popupMenu;
        
        popupMenu.addItem(1, "Load New Preset");
        popupMenu.addItem(2, "Load Default Preset");
        popupMenu.addItem(3, "Save Preset");
        
        return std::move(popupMenu);
    }
    DrawableButton *hamburger_button;
    PropertySet settings;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroAudioProcessorEditor)
};
