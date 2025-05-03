

#include "FilterButton.h"
#include "MyColours.h"
#include <BinaryData.h>

FilterButton::FilterButton(juce::RangedAudioParameter& param, juce::UndoManager* um)
    : audioParam(param)
    , paramAttachment(audioParam, [&](float v) { updateState(static_cast<bool> (v)); }, um)
{
    setWantsKeyboardFocus(true);
    setRepaintsOnMouseActivity(true);
    setColour(onColourId, MyColours::blue);
    setColour(offColourId, MyColours::midGrey);
    setColour(focusColourId, MyColours::midGrey.brighter(0.25f));

   

    offImage = juce::ImageCache::getFromMemory(BinaryData::BUTTONOFF_png, BinaryData::BUTTONOFF_pngSize);
    onImage = juce::ImageCache::getFromMemory(BinaryData::BUTTONON_png, BinaryData::BUTTONON_pngSize);

    jassert(!onImage.isNull());
    jassert(!offImage.isNull());

    paramAttachment.sendInitialUpdate();
}

void FilterButton::resized()
{
    iconBounds = getLocalBounds().toFloat();
    iconPath.applyTransform(iconPath.getTransformToScaleToFit(iconBounds, true));
}

void FilterButton::paint(juce::Graphics& g)
{
    // Choose the appropriate image based on the button's state.
    const juce::Image& currentImage = state ? onImage : offImage;

    if (!currentImage.isNull())
    {
        float imageW = static_cast<float>(currentImage.getWidth());
        float imageH = static_cast<float>(currentImage.getHeight());

        // Compute the factor that would exactly fit the image inside the button.
        float computedScale = std::min(getWidth() / imageW, getHeight() / imageH);

        // Apply an extra multiplier so the image appears larger.
        float multiplier = 7.0f;  // Adjust this value as desired.
        float scale = computedScale * multiplier;

        // Calculate the new dimensions.
        int destWidth = static_cast<int>(imageW * scale);
        int destHeight = static_cast<int>(imageH * scale);

        // Center the image within the button.
        int offsetX = (getWidth() - destWidth) / 2;
        int offsetY = (getHeight() - destHeight) / 2;

        // Draw the image with the new dimensions.
        g.drawImage(currentImage,
            offsetX, offsetY, destWidth, destHeight,   // destination rectangle
            0, 0, currentImage.getWidth(), currentImage.getHeight());  // source rectangle
    }
    else
    {
        // Fallback: fill with a solid colour if the image is missing.
        g.fillAll(state ? findColour(onColourId) : findColour(offColourId));
    }
}

void FilterButton::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);

    paramAttachment.setValueAsCompleteGesture(!state);
    
    const auto centre = iconBounds.getCentre();
    iconPath.applyTransform(juce::AffineTransform::scale(0.95f, 0.95f, centre.x, centre.y));
}

void FilterButton::mouseUp(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    iconPath.applyTransform(iconPath.getTransformToScaleToFit(iconBounds, true));
}

void FilterButton::focusGained(FocusChangeType cause)
{
    juce::ignoreUnused(cause);
    repaint();
}

void FilterButton::focusLost(FocusChangeType cause)
{
    juce::ignoreUnused(cause);
    repaint();
}

bool FilterButton::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        paramAttachment.setValueAsCompleteGesture(!state);
        return true;
    }

    return false;
}

void FilterButton::updateState(bool newState)
{
    state = newState;
    repaint();
}
