#pragma once
#include <JuceHeader.h>
using namespace juce;

class CombFilter
{
public:
    CombFilter() {}

    void prepare(const dsp::ProcessSpec& spec, float delayInMs)
    {
        sampleRate = spec.sampleRate;
        delayInMs = jlimit(1.0f, 50.0f, delayInMs);

        int maxDecaySamples = static_cast<int>((50.0f * sampleRate) / 1000.0f);
        int maxSpatialSamples = static_cast<int>((400.0f * sampleRate) / 1000.0f);

        decayDelayLine = dsp::DelayLine<float>(maxDecaySamples);
        spatialDelayLine = dsp::DelayLine<float>(maxSpatialSamples);
        widthDelayLine = dsp::DelayLine<float>(1024);

        decayDelayLine.prepare(spec);
        spatialDelayLine.prepare(spec);
        widthDelayLine.prepare(spec);

        reset();
    }

    float CombFilter::processSample(float inputSample, float feedbackGain, float sizeFactor, bool isLeftChannel, float width, float mix)
    {
        // clamp mix
        mix = juce::jlimit(0.0f, 1.0f, mix);

        
        float drySignal = inputSample;

        // decay
        float delayedFeedback = decayDelayLine.popSample(0);
        constexpr float cutoffFrequency = 2000.0f;
        float alpha = cutoffFrequency / (cutoffFrequency + sampleRate / (2.0f * MathConstants<float>::pi));
        delayedFeedback = alpha * delayedFeedback + (1.0f - alpha) * lastDecaySample;

        // attenuation to avoid runaway gain.
        delayedFeedback *= 0.99f;
        lastDecaySample = delayedFeedback;

        float decaySignal = (feedbackGain > 0.0f)
            ? inputSample + feedbackGain * delayedFeedback
            : inputSample;

        // spatial
        float spatialEcho = spatialDelayLine.popSample(0);
        constexpr float spatialFeedback = 0.45f;
        float spatialSignal = inputSample + spatialFeedback * spatialEcho;

        //  attenuation to stabilize
        spatialSignal *= 0.99f;
        spatialDelayLine.pushSample(0, spatialSignal);

        spatialEcho = (spatialEcho + lastSpatialSample) * 0.5f;
        lastSpatialSample = spatialEcho;

        decaySignal += 0.45f * spatialEcho;
        decayDelayLine.pushSample(0, decaySignal);

        // haas effect
        constexpr float maxWidthDelayTime = 0.02f; // 20 ms max delay at width = 1
        float delaySamples = width * (maxWidthDelayTime * sampleRate);
        widthDelayLine.setDelay(delaySamples);

        float delayedRight = widthDelayLine.popSample(0);
        widthDelayLine.pushSample(0, decaySignal);

        float leftOutput = decaySignal;
        float rightOutput = delayedRight;

        constexpr float spatialBlendFactor = 0.2f;
        leftOutput += spatialBlendFactor * spatialEcho;
        rightOutput += spatialBlendFactor * spatialEcho;

        constexpr float rightCorrectionFactor = 0.97f;
        rightOutput *= rightCorrectionFactor;

        float wetOutput = isLeftChannel ? leftOutput : rightOutput;

        //wet dry mix
        return (1.0f - mix) * drySignal + mix * wetOutput;
    }

    void setSize(float newSize, float baseDelayMs)
    {
        float spatialMultiplier = jmap(newSize, 0.0f, 1.0f, 1.0f, 15.0f);
        float spatialDelayMs = baseDelayMs * spatialMultiplier;
        int spatialSamples = static_cast<int>((spatialDelayMs * sampleRate) / 1000.0f);
        spatialDelayLine.setDelay(jlimit(1.0f,
            static_cast<float>(spatialDelayLine.getMaximumDelayInSamples()),
            static_cast<float>(spatialSamples)));

        float decayMultiplier = jmap(newSize, 0.0f, 1.0f, 1.0f, 1.25f);
        float decayDelayMs = baseDelayMs * decayMultiplier;
        int decaySamples = static_cast<int>((decayDelayMs * sampleRate) / 1000.0f);
        decayDelayLine.setDelay(jlimit(1.0f,
            static_cast<float>(decayDelayLine.getMaximumDelayInSamples()),
            static_cast<float>(decaySamples)));
    }

    void reset()
    {
        decayDelayLine.reset();
        spatialDelayLine.reset();
        widthDelayLine.reset();
        lastDecaySample = 0.0f;
        lastSpatialSample = 0.0f;
    }

private:
    double sampleRate = 44100.0;
    float lastDecaySample = 0.0f;
    float lastSpatialSample = 0.0f;

    dsp::DelayLine<float> decayDelayLine{ 44100 };
    dsp::DelayLine<float> spatialDelayLine{ 44100 };
    dsp::DelayLine<float> widthDelayLine{ 1024 };
};