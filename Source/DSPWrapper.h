#pragma once

#include "CustomReverb.h"
#include "OverlayFilterChain.h"
#include "VileFilter.h"
#include <JuceHeader.h>

class DSPWrapper
{
public:
    // Initialization and audio processing.
    void prepare(double sampleRate, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void setParameter(const juce::String& paramID, float value);


    void setOverlayMix(float newMix)
    {
        overlayChain.setMix(newMix);
    }

private:
    // DSP components.
    CustomReverb customReverb;
    OverlayFilterChain overlayChain;

    // Cached parameter values.
    float lastSize = 1.0f;
    float lastWidth = 1.0f;
    float lastMix = 0.5f;
    float lastDecay = 0.0f;  // Holds the DECAY slider value.

    // Overlay toggle: true if overlay processing is enabled.
    bool overlayOn = true;
};