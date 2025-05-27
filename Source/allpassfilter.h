#pragma once
#include <JuceHeader.h>

class AllPassFilter
{
public:
    AllPassFilter() {}

    void prepare(const juce::dsp::ProcessSpec& spec, float delayInMs)
    {
        sampleRate = spec.sampleRate;

        //delayInMs is within a valid range
        delayInMs = juce::jlimit(1.0f, 50.0f, delayInMs);

        // calculate delay in samples
        int delayInSamples = static_cast<int>(delayInMs * (sampleRate / 1000.0));

        // buffer size  and the maximum delay
        int maxDelayInSamples = static_cast<int>((50.0f * sampleRate) / 1000.0f);
        delayLine = juce::dsp::DelayLine<float>(maxDelayInSamples);

        
        delayLine.prepare(spec);

        // clamp delayInSamples to prevent exceeding the buffer size
        delayInSamples = juce::jlimit(0, delayLine.getMaximumDelayInSamples(), delayInSamples);

        delayLine.setDelay(delayInSamples);
    }

    float processSample(float inputSample, float gain)
    {
        gain = juce::jlimit(0.0f, 0.7f, gain); // allow higher gains for more audible effects
        float delayed = delayLine.popSample(0); // fetch delayed signal
        float output = -gain * inputSample + delayed; // calculate output
        float feedback = inputSample + gain * output; // feedback calculation
        delayLine.pushSample(0, feedback); // store feedback in the delay line
        return output; 
    }

    void reset()
    {
        delayLine.reset();
    }

private:
    double sampleRate = 44100.0;
    juce::dsp::DelayLine<float> delayLine{ 4410 }; // init with buffer size for delay line
};
