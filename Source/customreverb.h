#pragma once
#include "combfilter.h"
#include "allpassfilter.h"
#include <array>
#include <JuceHeader.h>
#include "FrequencyAnalyzer.h"  

class CustomReverb
{
public:
    
    CustomReverb(FrequencyAnalyzer* analyzer = nullptr)
        : frequencyAnalyzer(analyzer), fft(10)
    {
        
        demoTestSignal = false;
    }

    void prepare(double sampleRate, int numChannels)
    {
        this->sampleRate = sampleRate;

        instanceDecayBuffer.resize(512, 0.0f);


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

    float CustomReverb::processSample(float inputSample, float decay, bool isLeftChannel, float mix, int sampleIndex)
    {
        if (mix < 0.001f)
            return inputSample;

        float drySignal = inputSample;

        
        float noiseFilterFreq = juce::jmap(decay, 0.0f, 1.0f, 20.0f, 100.0f);
        float qFactor = 1.2f;
        juce::IIRFilter highPassFilter;
        highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(44100.0f, noiseFilterFreq, qFactor));
        float filteredInput = highPassFilter.processSingleSampleRaw(inputSample);

        
        float decayEffect = juce::jmap(decay, 0.0f, 1.0f, 0.2f, 0.75f);
        float decayInput = filteredInput * decayEffect;

       
        float adjustedSize = juce::jmap(sizeParameter, 0.0f, 1.0f, 0.7f, 1.3f);
        decayInput *= adjustedSize;

        
        float combSum = 0.0f;
        for (size_t i = 0; i < combFilters.size(); ++i)
        {
            
            combSum += combFilters[i].processSample(decayInput, 0.3f, sizeParameter, isLeftChannel, widthParameter, mix);
        }

        
        combSum = juce::jlimit(-0.5f, 0.5f, combSum);

       
        float allPassOut = combSum;
        for (size_t i = 0; i < allPassFilters.size(); ++i)
        {
            allPassOut = allPassFilters[i].processSample(allPassOut, 0.6f);
        }

        

        
        allPassOut *= juce::jmap(mix, 0.0f, 1.0f, 0.85f, 1.15f);

        float out = drySignal + allPassOut;
        return out;
    }

    void processBlock(juce::AudioBuffer<float>& buffer, float decay, float mix)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        
        if (numSamples == 0)
            return;

        if (buffer.hasBeenCleared())
        {
            std::fill(instanceDecayBuffer.begin(), instanceDecayBuffer.end(), 0.0f);
        }

        if (numChannels < 2)
            return;

        widthParameter = juce::jlimit(0.0f, 1.0f, widthParameter);
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);




       
        std::vector<float> monoBuffer(numSamples, 0.0f);

        if (demoTestSignal)
        {
            
            const float freq = 440.0f;
            for (int sample = 0; sample < numSamples; ++sample)
            {
                monoBuffer[sample] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * freq * sample / sampleRate);
                leftChannel[sample] = monoBuffer[sample];
                rightChannel[sample] = monoBuffer[sample];
            }
        }
        else
        {
            
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float leftWet = processSample(leftChannel[sample], decay, true, mix, sample);
                float rightWet = processSample(rightChannel[sample], decay, false, mix, sample);
                float monoSignal = (leftWet + rightWet) * 0.5f;

                
                leftChannel[sample] = juce::jmap(widthParameter, 0.0f, 1.0f, monoSignal, leftWet);
                rightChannel[sample] = juce::jmap(widthParameter, 0.0f, 1.0f, monoSignal, rightWet);
                monoBuffer[sample] = monoSignal;
            }
        }

       
    }

    void setSize(float newSize)
    {
        sizeParameter = juce::jlimit(0.4f, 2.8f, newSize);
        for (size_t i = 0; i < combFilters.size(); ++i)
            combFilters[i].setSize(sizeParameter, combDelaysMs[i]);
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
    juce::dsp::FFT fft;
    FrequencyAnalyzer* frequencyAnalyzer = nullptr;
    std::vector<float> instanceDecayBuffer; 
    
    bool demoTestSignal = false;
};