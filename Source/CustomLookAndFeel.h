#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    // Override the method that draws linear sliders.
    void drawLinearSlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    juce::Font getValueFont() const { return valueFont; }
    juce::Font getcustomTypeface() const { return customTypeface; }

private:
    // Stores textures as images (if you choose to use them later)
    juce::Image sliderTrack;
    juce::Image sliderKnob;

    juce::Font getLabelFont(juce::Label& label) override;

    juce::Typeface::Ptr customTypeface;
    juce::Font valueFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};