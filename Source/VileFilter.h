#pragma once
#include "OverlayFilter.h"
#include <cmath>

class VileFilter : public OverlayFilter
{
public:
    float processSample(float inputSample) override
    {
        // core distortion tanh-based saturation
        float distorted = std::tanh(inputSample * drive);

        // dynamic scraping: modulation based on signal amplitude
        float scrape = distorted * std::sin(inputSample * 20.0f); // adjust rate for texture

        // resonance so a feedback loop for metallic texture
        float lastSample = 0.0f;
        float resonance = (distorted + lastSample) * 0.5f; // feedback resonance
        lastSample = distorted;

        // blend scraping and resonance into the distortion
        float processed = distorted + 0.2f * scrape + 0.05f * resonance; // mix levels

        // normalize the output to reduce loudness
        processed *= 0.3f; // scale down amplitude

        
        return (1.0f - mix) * inputSample + mix * processed;
    }
};