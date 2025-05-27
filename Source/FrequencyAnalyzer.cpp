#include "FrequencyAnalyzer.h"

FrequencyAnalyzer::FrequencyAnalyzer()
{
    // Start with an empty (or zeroed) spectrum.
    spectrum.resize(100, 0.0f);
}

void FrequencyAnalyzer::paint(juce::Graphics& g)
{
    // Fill the background with a semi-transparent dark color
    g.fillAll(juce::Colour::fromRGBA(20, 20, 20, 150));

    if (!spectrum.empty())
    {
        // Choose a bright color for the spectrum display.
        g.setColour(juce::Colours::limegreen);

        const int numBins = static_cast<int>(spectrum.size());
        const float binWidth = static_cast<float>(getWidth()) / static_cast<float>(numBins);

        // Draw each frequency bin as a vertical bar.
        for (int i = 0; i < numBins; ++i)
        {
            // spectrum[i] is expected to be normalized: 0 to 1
            float barHeight = spectrum[i] * getHeight();
            // Draw a filled rectangle for this bin:
            g.fillRect(i * binWidth, getHeight() - barHeight, binWidth * 0.8f, barHeight);
        }
    }
    else
    {
        g.setColour(juce::Colours::red);
        g.drawFittedText("No Data", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void FrequencyAnalyzer::updateSpectrum(const std::vector<float>& newSpectrum)
{
    spectrum = newSpectrum; // Expecting values in the [0,1] range.
    repaint();
}