#pragma once
#include <JuceHeader.h>

class AllPassFilter
{
public:
    AllPassFilter() {}

    void prepare(const juce::dsp::ProcessSpec& spec, float delayInMs)
    {
        sampleRate = spec.sampleRate;

        // Ensure delayInMs is within a valid range
        delayInMs = juce::jlimit(1.0f, 50.0f, delayInMs);

        // Calculate delay in samples
        int delayInSamples = static_cast<int>(delayInMs * (sampleRate / 1000.0));

        // Ensure the buffer size can accommodate the maximum delay
        int maxDelayInSamples = static_cast<int>((50.0f * sampleRate) / 1000.0f);
        delayLine = juce::dsp::DelayLine<float>(maxDelayInSamples);

        // Prepare the delay line with the provided spec
        delayLine.prepare(spec);

        // Clamp delayInSamples to prevent exceeding the buffer size
        delayInSamples = juce::jlimit(0, delayLine.getMaximumDelayInSamples(), delayInSamples);

        delayLine.setDelay(delayInSamples);
    }

    float processSample(float inputSample, float gain)
    {
        gain = juce::jlimit(0.0f, 0.7f, gain); // Allow higher gains for more audible effects
        float delayed = delayLine.popSample(0); // Fetch delayed signal
        float output = -gain * inputSample + delayed; // Calculate output
        float feedback = inputSample + gain * output; // Feedback calculation
        delayLine.pushSample(0, feedback); // Store feedback in the delay line
        return output; // Return output
    }

    void reset()
    {
        delayLine.reset();
    }

private:
    double sampleRate = 44100.0;
    juce::dsp::DelayLine<float> delayLine{ 4410 }; // Initialize with buffer size for delay line
};
