#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "FilterButton.h"
#include "CustomLookAndFeel.h"

// Forward declaration of PluginEditor
class PluginEditor;

class EditorContent final : public juce::Component,
    public juce::Slider::Listener
{
public:
    EditorContent(PluginProcessor& p, juce::UndoManager& um, PluginEditor& editor);

    void resized() override;
    bool keyPressed(const juce::KeyPress& k) override;

    // Slider listener callback
    void sliderValueChanged(juce::Slider* slider) override;

private:



    juce::AudioProcessorValueTreeState& apvts;
    PluginEditor& pluginEditor;  // Reference to the PluginEditor

    // Sliders
    juce::Slider sizeSlider;
    juce::Slider dampSlider;
    juce::Slider widthSlider;
    juce::Slider mixSlider;

    // Attachments
    juce::AudioProcessorValueTreeState::SliderAttachment sizeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment dampAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment widthAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;

  


    juce::Label sizeLabel;
    juce::Label dampLabel;
    juce::Label widthLabel;
    juce::Label mixLabel;

    // Freeze button
    FilterButton filterButton;

    

    //OverlayFilter Slider
   // OverlayFilter& overlayFilter; // Reference to the OverlayFilter instance
    juce::Slider overlayBlendSlider;                // New slider for blending
    juce::Label overlayBlendLabel;                  // New label for the slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> overlayBlendAttachment;  // Parameter attachment
    

    //declare
    //std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    CustomLookAndFeel customLookAndFeel;


    juce::Colour interpolateColor(const juce::Colour& startColor, const juce::Colour& endColor, float ratio);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorContent)
};
