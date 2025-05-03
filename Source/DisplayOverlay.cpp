#include "DisplayOverlay.h"
#include "MainComponent.h"
#include <juce_opengl/juce_opengl.h>
using namespace juce::gl;


DisplayOverlay::DisplayOverlay() {}

void DisplayOverlay::paint(juce::Graphics& g)
{
    // Debug fill for visibility
    g.setColour(juce::Colours::blue.withAlpha(0.5f)); // Semi-transparent blue
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);

    // Draw the overlay image (if valid)
    if (overlayImage.isValid())
    {
        g.drawImageWithin(overlayImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }

    
}
void DisplayOverlay::setOverlayImage(const juce::Image& image)
{
    overlayImage = image;
    repaint(); // Redraw when the image changes
}
