#include "DSPWrapper.h"
#include <JuceHeader.h>
#include <cassert>
#include "ParameterIDs.h"

void DSPWrapper::prepare(double sampleRate, int numChannels)
{
    jassert(sampleRate > 0);
    jassert(numChannels > 0);

    
    customReverb.prepare(sampleRate, numChannels);
    overlayChain.setActiveFilter(std::make_unique<VileFilter>());
    overlayChain.setDrive(10.0f); 
}
void DSPWrapper::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    jassert(numChannels > 0 && numSamples > 0);

    // Create a working copy (wetBuffer) of the input buffer.
    juce::AudioBuffer<float> wetBuffer(buffer);

    // Always ensure wetBuffer starts as a copy of the dry signal
    wetBuffer.makeCopyOf(buffer);

    // Apply width and size parameters unconditionally
    customReverb.setWidth(lastWidth);
    customReverb.setSize(lastSize);

    // only process if mix > 0
    if (lastMix > 0.0f)
    {
        // process reverb, ensuring width and size are applied
        customReverb.processBlock(wetBuffer, lastDecay, lastMix);

        // apply overlay processing only if overlayOn is enabled
        if (overlayOn)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* wetChannel = wetBuffer.getWritePointer(channel);
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    wetChannel[sample] = overlayChain.processSample(wetChannel[sample]);
                }
            }
        }
    }

    // Combine the dry (original) and wet (processed) signals
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* dryChannel = buffer.getReadPointer(channel);    // Original signal
        auto* wetChannel = wetBuffer.getReadPointer(channel); // Processed signal
        auto* outputChannel = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            if (lastMix > 0.0f)
            {
                // Blend dry and wet signals based on lastMix
                outputChannel[sample] = (1.0f - lastMix) * dryChannel[sample] + lastMix * wetChannel[sample];
            }
            else
            {
                // Pass through dry signal untouched if mix is 0
                outputChannel[sample] = dryChannel[sample];
            }
        }
    }

    // Flush any lingering denormalized values
    buffer.applyGain(1.0f);
}

void DSPWrapper::setParameter(const juce::String& paramID, float value)
{
    // Handle parameters with consistent usage of ParameterIDs.
    if (paramID == ParameterIDs::size)
    {
        customReverb.setSize(value);
        lastSize = value;
    }
    else if (paramID == ParameterIDs::width)
    {
        customReverb.setWidth(value);
        lastWidth = value;
    }
    else if (paramID == ParameterIDs::mix)
    {
        overlayChain.setMix(value);
        lastMix = value;
    }
    else if (paramID == ParameterIDs::damp)
    {
        lastDecay = value;
    }
    else if (paramID.trim() == "OVERLAY_ON")
    {
        overlayOn = (value > 0.0f);
    }
    else
    {
        // Log unmatched paramID and trigger jassertfalse.
        jassertfalse; // Trigger breakpoint for unexpected parameter.
    }
}