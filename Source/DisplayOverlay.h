


#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class DisplayOverlay : public juce::Component
{
public:
    DisplayOverlay();
    void paint(juce::Graphics& g) override;

    void setOverlayImage(const juce::Image& image);  // setter for the PNG image.

private:
    juce::Image overlayImage; // store the PNG for the overlay.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisplayOverlay)
};
