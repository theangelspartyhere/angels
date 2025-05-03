#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "DisplayOverlay.h"

PluginEditor::PluginEditor(PluginProcessor& p, juce::UndoManager& um)
    : AudioProcessorEditor(&p),
    processor(p),
    undoManager(um),
    editorContent(p, um, *this),
    openGLComponent(),
    displayOverlay()
{
    setSize(350, 600);
    

    // Add components in the correct layering order
    addAndMakeVisible(openGLComponent);  // OpenGL cube component first
    addAndMakeVisible(editorContent);   // Sliders and other controls second
    

    //setSize(defaultWidth, defaultHeight); // Set the initial size
}


void PluginEditor::paint(juce::Graphics& g)
{

    auto backgroundImage = juce::ImageCache::getFromMemory(BinaryData::BACKGROUND_png, BinaryData::BACKGROUND_pngSize);

    // Check if the image was loaded correctly
    if (backgroundImage.isValid())
    {
        // Draw the image to fill the plugin's background area
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
    }
    else
    {
        // Fallback to a solid color if the image fails to load
        g.fillAll(juce::Colours::black);
    }

    

    auto area = getLocalBounds().toFloat();
    area.removeFromTop(30);

    // Make the rounded rectangle smaller (shorter height, narrower width)
    auto cutoutBounds = area.removeFromTop(150).withTrimmedLeft(75).withTrimmedRight(75);

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(cutoutBounds.expanded(10.0f), 20.0f); // Keep the corner radius
}

void PluginEditor::resized()
{
    //setSize(300, 600);
    auto area = getLocalBounds();

    // Add 10px padding at the top
    area.removeFromTop(30);

    // Match the OpenGL bounds to the smaller rounded rectangle
    auto openGLBounds = area.removeFromTop(150).withTrimmedLeft(75).withTrimmedRight(75);
    openGLComponent.setBounds(openGLBounds);

    // Remaining space for sliders and controls
    editorContent.setBounds(area);

    

}



bool PluginEditor::keyPressed(const juce::KeyPress& k)
{
    if (k.isKeyCode('Z') && k.getModifiers().isCommandDown())
    {
        if (k.getModifiers().isShiftDown())
            undoManager.redo();
        else
            undoManager.undo();

        return true;
    }

    return false;
}

void PluginEditor::updateCubeSize(float newSize)
{
    openGLComponent.setCubeSize(newSize);
}

void PluginEditor::updateDampValue(float newDamp)
{
    openGLComponent.setDampValue(newDamp);
}

void PluginEditor::updateWidthValue(float newWidth)
{
    openGLComponent.setWidthValue(newWidth);
}

void PluginEditor::updateCubeColor(juce::Colour newColor)
{
    openGLComponent.setCubeColor(newColor);
    
}
