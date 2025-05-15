#pragma once

#include "CustomReverb.h"
#include "OverlayFilterChain.h"
#include "VileFilter.h"
#include <JuceHeader.h>

class DSPWrapper
{
public:
    // init and audio processing.
    void prepare(double sampleRate, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void setParameter(const juce::String& paramID, float value);


    void setOverlayMix(float newMix)
    {
        overlayChain.setMix(newMix);
    }

private:
    
    CustomReverb customReverb;
    OverlayFilterChain overlayChain;

    // cached parameter values.
    float lastSize = 1.0f;
    float lastWidth = 1.0f;
    float lastMix = 0.5f;
    float lastDecay = 0.0f;  // holds the Ddecay slider value.

    // true if overlay processing  enabled.
    bool overlayOn = true;
};