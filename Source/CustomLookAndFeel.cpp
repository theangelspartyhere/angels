#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    // Load a custom typeface from binary data.
    customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::visitor2_ttf,
        BinaryData::visitor2_ttfSize);
    valueFont = juce::Typeface::createSystemTypefaceFor(BinaryData::visitor2_ttf,
        BinaryData::visitor2_ttfSize);
}

CustomLookAndFeel::~CustomLookAndFeel() {}

juce::Font CustomLookAndFeel::getLabelFont(juce::Label& label)
{
    // Return a custom font with a specific size and style.
    return juce::Font(customTypeface).withHeight(16.0f).withStyle(juce::Font::bold);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        // Draw only the filled portion of the slider.
        int filledWidth = static_cast<int>(sliderPos - x);
        g.setColour(juce::Colours::white); // Colour for the filled portion.
        g.fillRect(x, y, filledWidth, height);

        // Draw a thin outline around the entire slider area.
        // Disambiguate the drawRect() call by explicitly creating a juce::Rectangle<int>.
        //g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.setColour(juce::Colours::white);
        g.drawRect(juce::Rectangle<int>(x, y, width, height), 1.3f);
    }
    else
    {
        // For other slider styles, fall back to the default drawing.
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 0, 0, style, slider);
    }
}