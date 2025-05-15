#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    // load a custom typeface from binary data.
    customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::visitor2_ttf,
        BinaryData::visitor2_ttfSize);
    valueFont = juce::Typeface::createSystemTypefaceFor(BinaryData::visitor2_ttf,
        BinaryData::visitor2_ttfSize);
}

CustomLookAndFeel::~CustomLookAndFeel() {}

juce::Font CustomLookAndFeel::getLabelFont(juce::Label& label)
{
    // custom font 
    return juce::Font(customTypeface).withHeight(16.0f).withStyle(juce::Font::bold);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        // draw only the filled portion of the slider.
        int filledWidth = static_cast<int>(sliderPos - x);
        g.setColour(juce::Colours::white); // colour for the filled portion.
        g.fillRect(x, y, filledWidth, height);

        
        //g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.setColour(juce::Colours::white);
        g.drawRect(juce::Rectangle<int>(x, y, width, height), 1.3f);
    }
    else
    {
        
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 0, 0, style, slider);
    }
}