#include "EditorContent.h"
#include "ParameterIDs.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

// EditorContent constructor
EditorContent::EditorContent(PluginProcessor& p, juce::UndoManager& um, PluginEditor& editor)
    : apvts(p.getPluginState()),
    pluginEditor(editor),
    sizeSlider("Size"),
    dampSlider("Damp"),
    widthSlider("Width"),
    mixSlider("Mix"),
    sizeAttachment(apvts, ParameterIDs::size, sizeSlider),
    dampAttachment(apvts, ParameterIDs::damp, dampSlider),
    widthAttachment(apvts, ParameterIDs::width, widthSlider),
    mixAttachment(apvts, ParameterIDs::mix, mixSlider),
    filterButton(*apvts.getParameter("OVERLAY_ON"), &um)
{
    setWantsKeyboardFocus(true);
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);

    // Ensure sliders reflect the initial parameter state (restored via APVTS)
    sizeSlider.setValue(apvts.getParameterAsValue(ParameterIDs::size).getValue());
    dampSlider.setValue(apvts.getParameterAsValue(ParameterIDs::damp).getValue());
    widthSlider.setValue(apvts.getParameterAsValue(ParameterIDs::width).getValue());
    mixSlider.setValue(apvts.getParameterAsValue(ParameterIDs::mix).getValue());
    //mixSlider.setValue(100.0f, juce::dontSendNotification);

    // Optionally, synchronize labels with restored slider values
    sizeLabel.setText(juce::String(sizeSlider.getValue(), 1), juce::dontSendNotification);
    dampLabel.setText(juce::String(dampSlider.getValue(), 1), juce::dontSendNotification);
    widthLabel.setText(juce::String(widthSlider.getValue(), 1), juce::dontSendNotification);
    //mixLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);

    // Slider aesthetics and listeners
    sizeSlider.setLookAndFeel(&customLookAndFeel);
    dampSlider.setLookAndFeel(&customLookAndFeel);
    widthSlider.setLookAndFeel(&customLookAndFeel);
    mixSlider.setLookAndFeel(&customLookAndFeel);

    // Set slider ranges and listeners
    sizeSlider.setRange(0.0, 100.0, 1.0);
    sizeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    sizeSlider.addListener(this);
    addAndMakeVisible(sizeSlider);

    dampSlider.setRange(0.0, 100.0, 1.0);
    dampSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    dampSlider.addListener(this);
    addAndMakeVisible(dampSlider);

    widthSlider.setRange(0.0, 100.0, 1.0);
    widthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    widthSlider.addListener(this);
    addAndMakeVisible(widthSlider);

    mixSlider.setRange(0.0, 100.0, 1.0);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    mixSlider.addListener(this);
    addAndMakeVisible(mixSlider);

   
    filterButton.setExplicitFocusOrder(5);

    
    addAndMakeVisible(filterButton);

    // Now hide the mix slider and filter button
    mixSlider.setVisible(false);
    filterButton.setVisible(false);


   

    // Set up labels
    /*sizeLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(18.0f).withExtraKerningFactor(-0.1f));
    sizeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sizeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(sizeLabel);

    dampLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(18.0f).withExtraKerningFactor(-0.1f));
    dampLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dampLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(dampLabel);

    widthLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(18.0f).withExtraKerningFactor(-0.1f));
    widthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    widthLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(widthLabel);

    mixLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(18.0f).withExtraKerningFactor(-0.1f));
    mixLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mixLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mixLabel);*/

    //OVERLAYSLIDER
    overlayBlendSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    overlayBlendSlider.setRange(0.0, 100.0, 0.1);  // Use percentage for range
    overlayBlendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);  // Hide textbox
    overlayBlendSlider.setLookAndFeel(&customLookAndFeel);
    overlayBlendSlider.addListener(this);  // Listen for changes
    addAndMakeVisible(overlayBlendSlider);

    overlayBlendLabel.setFont(customLookAndFeel.getcustomTypeface().withHeight(13.0f));
    overlayBlendLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    overlayBlendLabel.setJustificationType(juce::Justification::centredRight);
    //overlayBlendLabel.setText("OVERLAY BLEND", juce::dontSendNotification);
    addAndMakeVisible(overlayBlendLabel);

    overlayBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "OVERLAY_BLEND", overlayBlendSlider);

}


 

void EditorContent::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Define the height for the icon
    auto iconHeight = 50; // Set a reasonable height for the icon
    auto iconArea = area.removeFromTop(iconHeight);

 

    const int spacer = 50;
    area.removeFromTop(spacer);

    // Allocate space for sliders
    auto sliderArea = area;
    //auto sliderHeight = sliderArea.getHeight() / 4;
   // Example: Explicit dimensions for smaller, thinner sliders
    const int sliderWidth = 150;  // Keep the narrow width
    const int sliderHeight = 14; // Keep the slim height
    const int sliderSpacing = 21; // Reduced spacing between sliders

    // Move the starting X position further to the right
    const int startingX = 100; // Adjust this value to move sliders to the right
    const int startingY = 150; // Maintain the lower starting Y position
    
    const int labelWidth = 30;  // Width of the value label
    const int labelOffset = 5; // Space between slider and label

    sizeSlider.setBounds(startingX, startingY, sliderWidth, sliderHeight);
    sizeLabel.setBounds(startingX + sliderWidth + labelOffset - 7 , startingY, labelWidth, sliderHeight);

    dampSlider.setBounds(startingX, startingY + sliderHeight + sliderSpacing, sliderWidth, sliderHeight);
    dampLabel.setBounds(startingX + sliderWidth + labelOffset - 7, startingY + sliderHeight + sliderSpacing, labelWidth, sliderHeight);

    widthSlider.setBounds(startingX, startingY + (sliderHeight + sliderSpacing) * 2, sliderWidth, sliderHeight);
    widthLabel.setBounds(startingX + sliderWidth + labelOffset -7 , startingY + (sliderHeight + sliderSpacing) * 2, labelWidth, sliderHeight);

    mixSlider.setBounds(startingX, startingY + (sliderHeight + sliderSpacing) * 3, sliderWidth, sliderHeight);
    mixLabel.setBounds(startingX + sliderWidth + labelOffset -7, startingY + (sliderHeight + sliderSpacing) * 3, labelWidth, sliderHeight);



    // Center the logo (freeze button)
    auto logoWidth = 128;
    auto logoHeight = 96;
    filterButton.setBounds(iconArea.withSizeKeepingCentre(logoWidth, logoHeight));




    // Overlay Blend Slide
    //auto filterButtonBounds = filterButton.getBounds(); // Get freeze button bounds
    auto overlayArea = getLocalBounds().reduced(10).removeFromBottom(40); // Adjust positioning manually

    //const int overlaySliderHeight = 12; // Make the slider thinner
    //overlayBlendSlider.setBounds(
    //    filterButtonBounds.getX(),              // Same X as the freeze button
    //    filterButtonBounds.getBottom() + 10,    // Positioned directly below the freeze button
    //    filterButtonBounds.getWidth(),          // Same width as the freeze button
    //    overlaySliderHeight                     // Thinner height
    //);
    const int overlayX = 120;  // Adjust based on previous button position
    const int overlayY = 310; // Keep it near the original spot
    const int overlayWidth = 110; // Keep width manageable
    const int overlayHeight = 12; // Maintain slim slider height

    overlayBlendSlider.setBounds(overlayX, overlayY, overlayWidth, overlayHeight);

    // Optional: Position the label below or next to the slider (adjust as needed)
    //overlayBlendLabel.setBounds(
    //    overlayBlendSlider.getX() - 2,
    //    overlayBlendSlider.getBottom() + 5,     // Add spacing below the slider
    //    overlayBlendSlider.getWidth(),
    //    overlaySliderHeight                     // Same height as the slider for consistency
    //);

}

void EditorContent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sizeSlider)
    {
        float sliderValue = sizeSlider.getValue() / 100.0f;  // Normalize to [0.0, 1.0]
        float sizeValue = 1.0f + (sliderValue * 1.0f);        // Map to [0.5, 2.0]
        sizeLabel.setText(juce::String(sizeSlider.getValue(), 1), juce::dontSendNotification); // One decimal place


        // Notify PluginEditor about the size change
        pluginEditor.updateCubeSize(sizeValue);
    }
    else if (slider == &dampSlider)
    {
        float sliderValue = dampSlider.getValue() / 100.0f;  // Normalize to [0.0, 1.0]
        dampLabel.setText(juce::String(dampSlider.getValue(), 1), juce::dontSendNotification);


        // Notify PluginEditor about the damp change
        pluginEditor.updateDampValue(sliderValue);
    }
    else if (slider == &widthSlider)
    {
        float sliderValue = widthSlider.getValue() / 100.0f;  // Normalize to [0.0, 1.0]
        float widthValue = (sliderValue );       // Map to [-1.0, 1.0]
        widthLabel.setText(juce::String(widthSlider.getValue(), 1), juce::dontSendNotification);


        // Notify PluginEditor about the width change
        pluginEditor.updateWidthValue(widthValue);
    }

    mixLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);
    // Handle mixSlider if needed...
}

bool EditorContent::keyPressed(const juce::KeyPress& k)
{
    if (k.isKeyCode(juce::KeyPress::tabKey) && hasKeyboardFocus(false))
    {
        
        return true;
    }

    return false;
}

juce::Colour EditorContent::interpolateColor(const juce::Colour& startColor, const juce::Colour& endColor, float ratio)
{
    return startColor.interpolatedWith(endColor, ratio);
}

