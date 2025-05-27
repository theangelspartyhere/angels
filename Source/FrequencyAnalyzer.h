#pragma once
#include <JuceHeader.h>
#include <vector>

class FrequencyAnalyzer : public juce::Component
{
public:
    FrequencyAnalyzer();
    void paint(juce::Graphics& g) override;
    void updateSpectrum(const std::vector<float>& newSpectrum);

private:
    std::vector<float> spectrum;
};