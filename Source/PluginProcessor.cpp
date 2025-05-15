#include "PluginProcessor.h"
#include "ParameterIDs.h"
#include "PluginEditor.h"


static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const auto percentageAttributes = juce::AudioParameterFloatAttributes().withStringFromValueFunction(
        [](auto value, auto)
        {
            constexpr auto unit = " %";
            if (auto v{ std::round(value * 100.0f) / 100.0f }; v < 10.0f)
                return juce::String{ v, 2 } + unit;
            if (auto v{ std::round(value * 10.0f) / 10.0f }; v < 100.0f)
                return juce::String{ v, 1 } + unit;
            return juce::String{ std::round(value) } + unit;
        });

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParameterIDs::size, 1 },
        ParameterIDs::size,
        juce::NormalisableRange<float>{ 0.0f, 100.0f, 0.01f, 1.0f },
        0.0f,
        percentageAttributes));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParameterIDs::damp, 1 },
        ParameterIDs::damp,
        juce::NormalisableRange<float>{ 0.0f, 100.0f, 0.01f, 1.0f },
        0.0f,
        percentageAttributes));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParameterIDs::width, 1 },
        ParameterIDs::width,
        juce::NormalisableRange<float>{ 0.0f, 100.0f, 0.01f, 1.0f },
        0.0f,
        percentageAttributes));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParameterIDs::mix, 1 },
        ParameterIDs::mix,
        juce::NormalisableRange<float>{ 0.0f, 100.0f, 0.01f, 1.0f },
        100.0f,
        percentageAttributes));

    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "OVERLAY_BLEND", 1 },
        "Overlay Blend",
        juce::NormalisableRange<float>{ 0.0f, 100.0f, 0.01f, 1.0f },
        50.0f,  // default value set at 50%
        percentageAttributes));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "OVERLAY_ON", 1 }, "Overlay On", true));

    return layout;
}

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, &undoManager, "Parameters", createParameterLayout())
{
    size = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParameterIDs::size));
    damp = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParameterIDs::damp));
    width = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParameterIDs::width));
    mix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParameterIDs::mix));
    //freeze = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(ParameterIDs::freeze));

    jassert(size != nullptr);
    jassert(damp != nullptr);
    jassert(width != nullptr);
    jassert(mix != nullptr);
    //jassert(freeze != nullptr);
}

const juce::String PluginProcessor::getName() const { return JucePlugin_Name; }

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String PluginProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
void PluginProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    

    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    dspWrapper.prepare(sampleRate, getTotalNumInputChannels());
    limiter.prepare(spec);
    limiter.reset();
    limiter.setThreshold(-6.0f);
    limiter.setRelease(200.0f);
}

void PluginProcessor::releaseResources()
{
    
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // scoped guard against denormalized floating-point numbers
    juce::ScopedNoDenormals noDenormals;

    juce::ignoreUnused(midiMessages); // prevent compiler warnings for unused parameters

    // set DSP params
    dspWrapper.setParameter(ParameterIDs::size, juce::jlimit(0.0f, 1.0f, size->get() * 0.01f));
    dspWrapper.setParameter(ParameterIDs::damp, juce::jlimit(0.0f, 1.0f, damp->get() * 0.01f));
    dspWrapper.setParameter(ParameterIDs::width, juce::jlimit(0.0f, 1.0f, width->get() * 0.01f));
    dspWrapper.setParameter(ParameterIDs::mix, juce::jlimit(0.0f, 1.0f, mix->get() * 0.01f));
    dspWrapper.setParameter("OVERLAY_ON", apvts.getRawParameterValue("OVERLAY_ON")->load());


    //update the overlay filter mix based on the overlay parameter.
    if (auto* blendParam = apvts.getRawParameterValue("OVERLAY_BLEND"))
    {
        //  normalize param range it to 0.0–1.0:
        float blendNormalized = blendParam->load() * 0.01f;
        dspWrapper.setOverlayMix(blendNormalized);
    }
    else
    {
        jassertfalse; // overlay parameter not found
    }



    // process the audio block with the DSPWrapper
    dspWrapper.processBlock(buffer);

   


    // limiter 
    juce::dsp::AudioBlock<float> limiterBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> limiterCtx(limiterBlock);
    //limiter.process(limiterCtx);


    buffer.applyGain(0.75f); 

    // soft clipping with tanh
    const float clipFactor = 0.7f;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = std::tanh(channelData[sample] * clipFactor) / std::tanh(clipFactor);
        }
    }

    // flush denormalized values
    buffer.applyGain(0.7f);

   
}

bool PluginProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* PluginProcessor::createEditor() { return new PluginEditor(*this, undoManager); }

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

    
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (const auto tree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes)); tree.isValid())
    {
        apvts.replaceState(tree);
        
    }
    else
    {
       
    }
}

juce::AudioProcessorValueTreeState& PluginProcessor::getPluginState() { return apvts; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new PluginProcessor(); }