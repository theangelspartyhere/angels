#pragma once
#include "OverlayFilter.h"
#include <cmath>

class VileFilter : public OverlayFilter
{
public:
    float processSample(float inputSample) override
    {
        // Core distortion: Apply tanh-based saturation
        float distorted = std::tanh(inputSample * drive);

        // Dynamic scraping: Fast modulation based on signal amplitude
        float scrape = distorted * std::sin(inputSample * 20.0f); // Adjust rate for texture

        // Dirty resonance: Feedback loop for metallic texture
        static float lastSample = 0.0f;
        float resonance = (distorted + lastSample) * 0.5f; // Simple feedback resonance
        lastSample = distorted;

        // Blend scraping and resonance into the distortion
        float processed = distorted + 0.2f * scrape + 0.05f * resonance; // Adjust mix levels

        // Normalize the output to reduce loudness
        processed *= 0.3f; // Scale down the amplitude (adjust the factor as needed)

        // Final dry/wet blending (use "processed" directly)
        return (1.0f - mix) * inputSample + mix * processed;
    }
};