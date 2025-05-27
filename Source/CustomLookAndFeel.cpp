#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    // load a custom typeface from binary data.
    customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::bold_ttf,
        BinaryData::bold_ttfSize);
    valueFont = juce::Typeface::createSystemTypefaceFor(BinaryData::bold_ttf,
        BinaryData::bold_ttfSize);
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
    if (style == juce::Slider::LinearVertical)
    {
        // Calculate fill height from bottom to sliderPos
        int filledHeight = static_cast<int>(sliderPos - y);

        
        g.setColour(juce::Colours::white);
        g.fillRect(x, y + filledHeight, width, height - filledHeight);

        
        g.setColour(juce::Colours::white);
        g.drawRect(juce::Rectangle<int>(x, y, width, height), 1.3f);
    }
    else if (style == juce::Slider::LinearHorizontal)
    {
        
        int filledWidth = static_cast<int>(sliderPos - x);
        g.setColour(juce::Colours::white);
        g.fillRect(x, y, filledWidth, height);
        g.setColour(juce::Colours::white);
        g.drawRect(juce::Rectangle<int>(x, y, width, height), 1.3f);
    }
    else
    {
        
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 0, 0, style, slider);
    }
}

