

#pragma once


#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPWrapper.h"

class PluginProcessor final : public juce::AudioProcessor
{
public:

    PluginProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getPluginState();

private:
    juce::AudioProcessorValueTreeState apvts;

    juce::AudioParameterFloat* size{ nullptr };
    juce::AudioParameterFloat* damp{ nullptr };
    juce::AudioParameterFloat* width{ nullptr };
    juce::AudioParameterFloat* mix{ nullptr };
    juce::AudioParameterBool* freeze{ nullptr };

    void updateReverbParams();

    juce::dsp::Reverb::Parameters params;
    juce::dsp::Reverb reverb;
    juce::dsp::Limiter<float> limiter;





    juce::UndoManager undoManager;

    DSPWrapper dspWrapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};


