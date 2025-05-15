#include "DisplayOverlay.h"
#include "MainComponent.h"
#include <juce_opengl/juce_opengl.h>
using namespace juce::gl;


DisplayOverlay::DisplayOverlay() {}

void DisplayOverlay::paint(juce::Graphics& g)
{
    
    g.setColour(juce::Colours::blue.withAlpha(0.5f)); // semitransparent blue
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);

    // draw the overlay image (if valid)
    if (overlayImage.isValid())
    {
        g.drawImageWithin(overlayImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }

    
}
void DisplayOverlay::setOverlayImage(const juce::Image& image)
{
    overlayImage = image;
    repaint(); // redraw when the image changes
}
