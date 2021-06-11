/*
  ==============================================================================

    ESLookAndFeel.h
    Created: 17 Mar 2021 1:22:06pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ESLookAndFeel : public LookAndFeel_V4
{
public:
    ESLookAndFeel();
    ~ESLookAndFeel();
    
    Font getPopupMenuFont (void) override;
    
    Font getTextButtonFont (TextButton& b, int buttonHeight) override;
    
    Font getLabelFont (Label& label) override;
    
    void drawButtonBackground (Graphics& g,
                               Button& button,
                               const Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;
    
    void drawButtonText (Graphics& g, TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override;
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override;
    
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    
    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                            const bool isSeparator, const bool isActive,
                            const bool isHighlighted, const bool isTicked,
                            const bool hasSubMenu, const String& text,
                            const String& shortcutKeyText,
                            const Drawable* icon, const Colour* const textColourToUse) override;
    
    int getTabButtonBestWidth(TabBarButton & button, int tabDepth) override;
    
    Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp) override;
    
    void drawTableHeaderColumn (Graphics& g, TableHeaderComponent& header,
                                const String& columnName, int /*columnId*/,
                                int width, int height, bool isMouseOver, bool isMouseDown,
                                int columnFlags) override;
};
