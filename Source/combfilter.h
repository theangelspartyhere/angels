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

        int maxDecaySamples = static_cast<int>((150.0f * sampleRate) / 1000.0f);
        int maxSpatialSamples = static_cast<int>((600.0f * sampleRate) / 1000.0f);

        decayDelayLine = dsp::DelayLine<float>(maxDecaySamples);
        spatialDelayLine = dsp::DelayLine<float>(maxSpatialSamples);
        widthDelayLine = dsp::DelayLine<float>(1024);

        decayDelayLine.prepare(spec);
        spatialDelayLine.prepare(spec);
        widthDelayLine.prepare(spec);

       
        highPassDecayFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, decayCutoffFrequency, 0.707f));

        reset();
    }
    float processSample(float inputSample, float feedbackGain, float sizeFactor,
        bool isLeftChannel, float width, float mix)
    {
        if (mix < 0.001f)
            return inputSample;

        mix = juce::jlimit(0.0f, 1.0f, mix);
        float drySignal = inputSample;

        
        float decayInput = highPassDecayFilter.processSingleSampleRaw(inputSample);

        
        float delayedFeedback = decayDelayLine.popSample(0);
        constexpr float combCutoff = 2000.0f;
        float alpha = combCutoff / (combCutoff + sampleRate / (2.0f * juce::MathConstants<float>::pi));
        delayedFeedback = alpha * delayedFeedback + (1.0f - alpha) * lastDecaySample;
        delayedFeedback *= 0.9995f;
        lastDecaySample = delayedFeedback;

       
        if (std::abs(sizeFactor - lastSizeFactor) > 0.001f)
        {
            float sizeMapped = juce::jmap(sizeFactor, 0.0f, 1.0f, 0.5f, 2.5f);
            int maxDelaySamples = decayDelayLine.getMaximumDelayInSamples();
            int newDelay = juce::jlimit(1, maxDelaySamples,
                static_cast<int>(sizeMapped * sampleRate / 1000.0f));
            decayDelayLine.setDelay(newDelay);
            lastSizeFactor = sizeFactor;
        }

        
        float decayTail = (feedbackGain > 0.0f)
            ? feedbackGain * delayedFeedback * 1.1f
            : 0.0f;
        
        float decaySignal = decayInput * 0.1f + decayTail;

        
        float spatialEcho = spatialDelayLine.popSample(0);
        
        float spatialSignal = decayInput + 0.45f * spatialEcho;
        spatialSignal *= 0.99f;
        spatialDelayLine.pushSample(0, spatialSignal);
        spatialEcho = (spatialEcho + lastSpatialSample) * 0.5f;
        lastSpatialSample = spatialEcho;

       
        decaySignal += 0.2f * spatialEcho;
        decayDelayLine.pushSample(0, decaySignal);

        
        constexpr float maxWidthDelayTime = 0.02f;
        float delaySamples = width * (maxWidthDelayTime * sampleRate);
        widthDelayLine.setDelay(delaySamples);
        float delayedRight = widthDelayLine.popSample(0);
        widthDelayLine.pushSample(0, decaySignal);

        float leftOutput = decaySignal;
        float rightOutput = delayedRight;
        constexpr float spatialBlendFactor = 0.20f;
        leftOutput += spatialBlendFactor * spatialEcho;
        rightOutput += spatialBlendFactor * spatialEcho;
        constexpr float rightCorrectionFactor = 0.97f;
        rightOutput *= rightCorrectionFactor;

        float wetOutput = isLeftChannel ? leftOutput : rightOutput;

       
        float out = (1.0f - mix) * drySignal + mix * wetOutput;
        return out;
    }

    void setSize(float newSize, float baseDelayMs)
    {
        float spatialMultiplier = jmap(newSize, 0.0f, 1.0f, 1.0f, 15.0f);
        float spatialDelayMs = baseDelayMs * spatialMultiplier;
        int spatialSamples = static_cast<int>((spatialDelayMs * sampleRate) / 1000.0f);
        spatialDelayLine.setDelay(jlimit(1.0f,
            static_cast<float>(spatialDelayLine.getMaximumDelayInSamples()),
            static_cast<float>(spatialSamples)));

        float decayMultiplier = jmap(newSize, 0.0f, 1.0f, 1.0f, 1.5f);
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

    
    float lastSizeFactor = -1.0f;  
    
    juce::IIRFilter highPassDecayFilter;
    float decayCutoffFrequency = 120.0f; 

    dsp::DelayLine<float> decayDelayLine{ 44100 };
    dsp::DelayLine<float> spatialDelayLine{ 44100 };
    dsp::DelayLine<float> widthDelayLine{ 1024 };
};