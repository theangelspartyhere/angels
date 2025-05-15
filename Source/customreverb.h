#pragma once
#include "combfilter.h"
#include "allpassfilter.h"
#include <array>
#include <JuceHeader.h>

class CustomReverb
{
public:
    CustomReverb() {}

    void prepare(double sampleRate, int numChannels)
    {
        this->sampleRate = sampleRate;
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.numChannels = static_cast<juce::uint32>(numChannels);
        spec.maximumBlockSize = 512;

        sizeParameter = juce::jlimit(0.0f, 1.0f, sizeParameter);

        for (size_t i = 0; i < combFilters.size(); ++i)
        {
            float baseDelayInMs = combDelaysMs[i];
            float scaledDelayInMs = juce::jmap(sizeParameter, 0.5f, 2.0f) * baseDelayInMs;
            combFilters[i].prepare(spec, scaledDelayInMs);
        }

        for (size_t i = 0; i < allPassFilters.size(); ++i)
        {
            float baseDelayInMs = allPassDelaysMs[i];
            float scaledDelayInMs = juce::jmap(sizeParameter, 0.5f, 2.0f) * baseDelayInMs;
            allPassFilters[i].prepare(spec, scaledDelayInMs);
        }
    }

    float processSample(float inputSample, float decay, bool isLeftChannel, float mix)
    {
        decay = juce::jlimit(0.0f, 1.0f, decay);
        decay *= 0.99f; //decay to 99%
        float combSum = 0.0f;

        for (size_t i = 0; i < combFilters.size(); ++i)
        {
            float adjustedDecay = std::pow(decay, 0.07f);
            float feedbackGain = juce::jmap(adjustedDecay, 0.0f, 1.0f, 0.15f, 1.02f); //increased decay strength  

            if (!isLeftChannel)
                feedbackGain *= 0.98f;

            combSum += combFilters[i].processSample(inputSample, feedbackGain, sizeParameter, isLeftChannel, widthParameter, mix)
                * (1.0f + sizeParameter * 0.5f);
        }

        combSum /= static_cast<float>(combFilters.size());
        combSum = tanh(combSum);

        float allPassOut = combSum;
        for (size_t i = 0; i < allPassFilters.size(); ++i)
        {
            float gain = (decay > 0.0f) ? 0.65f : 0.0f; // boost diffusion strength  
            if (!isLeftChannel)
                gain *= 1.02f;
            allPassOut = allPassFilters[i].processSample(allPassOut, gain);
        }

        return allPassOut;
    }

    void processBlock(juce::AudioBuffer<float>& buffer, float decay, float mix)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        if (numChannels < 2)
            return;

        
        widthParameter = juce::jlimit(0.0f, 1.0f, widthParameter);

        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float leftWet = processSample(leftChannel[sample], decay, true, mix);
            float rightWet = processSample(rightChannel[sample], decay, false, mix);
            float monoSignal = (leftWet + rightWet) * 0.5f;

            //widthparameter to interpolate between a fully mono signal and the processed stereo signal.
            leftChannel[sample] = juce::jmap(widthParameter, 0.0f, 1.0f, monoSignal, leftWet);
            rightChannel[sample] = juce::jmap(widthParameter, 0.0f, 1.0f, monoSignal, rightWet);
        }
    }

    void setSize(float newSize)
    {
        sizeParameter = juce::jlimit(0.4f, 2.8f, newSize); // expanded size range
        for (size_t i = 0; i < combFilters.size(); ++i)
        {
            combFilters[i].setSize(sizeParameter, combDelaysMs[i]);
        }
    }

    void setWidth(float newWidth)
    {
        widthParameter = juce::jlimit(0.0f, 1.0f, newWidth);
    }

    void reset()
    {
        for (auto& comb : combFilters)
            comb.reset();
        for (auto& ap : allPassFilters)
            ap.reset();
    }




private:
    double sampleRate = 44100.0;
    const std::array<float, 8> combDelaysMs = { 15.0f, 17.0f, 19.0f, 21.0f, 25.0f, 26.6f, 28.9f, 30.8f };
    const std::array<float, 2> allPassDelaysMs = { 11.6f, 9.2f };
    std::array<CombFilter, 8> combFilters;
    std::array<AllPassFilter, 2> allPassFilters;
    float sizeParameter = 1.0f;
    float widthParameter = 1.0f;
};