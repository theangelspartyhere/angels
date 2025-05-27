#pragma once

#include <JuceHeader.h>
#include "MainComponent.h"
#include "PluginProcessor.h"
#include "EditorContent.h"
#include "DisplayOverlay.h"

class PluginEditor final : public juce::AudioProcessorEditor
{
public:
    PluginEditor(PluginProcessor& p, juce::UndoManager& um);

    void resized() override;
    void paint(juce::Graphics& g) override;
    bool keyPressed(const juce::KeyPress& k) override;

    //  update visualization parameters
    void updateCubeSize(float newSize);
    void updateDampValue(float newDamp);
    void updateWidthValue(float newWidth);
    void updateCubeColor(juce::Colour newColor);



private:
    PluginProcessor& processor;
    juce::UndoManager& undoManager;

    DisplayOverlay displayOverlay;

    MainComponent openGLComponent;
    EditorContent editorContent;

    static constexpr auto defaultWidth{ 350 };
    static constexpr auto defaultHeight{ 600 };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};