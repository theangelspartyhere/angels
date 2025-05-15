#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "FilterButton.h"
#include "CustomLookAndFeel.h"

// forward declaration 
class PluginEditor;

class EditorContent final : public juce::Component,
    public juce::Slider::Listener
{
public:
    EditorContent(PluginProcessor& p, juce::UndoManager& um, PluginEditor& editor);

    void resized() override;
    bool keyPressed(const juce::KeyPress& k) override;

    // slider listener callback
    void sliderValueChanged(juce::Slider* slider) override;

private:



    juce::AudioProcessorValueTreeState& apvts;
    PluginEditor& pluginEditor;  // reference to plugineditor

    
    juce::Slider sizeSlider;
    juce::Slider dampSlider;
    juce::Slider widthSlider;
    juce::Slider mixSlider;

    //attachments
    juce::AudioProcessorValueTreeState::SliderAttachment sizeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment dampAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment widthAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;

  


    juce::Label sizeLabel;
    juce::Label dampLabel;
    juce::Label widthLabel;
    juce::Label mixLabel;

    
    FilterButton filterButton;

    

    //OverlayFilter Slider
   // OverlayFilter& overlayFilter; // reference to the overlayfilter
    juce::Slider overlayBlendSlider;                // slider for blending
    juce::Label overlayBlendLabel;                  // label for the slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> overlayBlendAttachment;  // param attachment
    

    //declare
    //std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    CustomLookAndFeel customLookAndFeel;


    juce::Colour interpolateColor(const juce::Colour& startColor, const juce::Colour& endColor, float ratio);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorContent)
};
