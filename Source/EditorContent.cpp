#include "EditorContent.h"
#include "ParameterIDs.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

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
    mixAttachment(apvts, ParameterIDs::mix, mixSlider)
{
    setWantsKeyboardFocus(true);
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);


    sizeSlider.setValue(apvts.getParameterAsValue(ParameterIDs::size).getValue());
    dampSlider.setValue(apvts.getParameterAsValue(ParameterIDs::damp).getValue());
    widthSlider.setValue(apvts.getParameterAsValue(ParameterIDs::width).getValue());
    mixSlider.setValue(apvts.getParameterAsValue(ParameterIDs::mix).getValue());


    sizeSlider.setLookAndFeel(&customLookAndFeel);
    dampSlider.setLookAndFeel(&customLookAndFeel);
    widthSlider.setLookAndFeel(&customLookAndFeel);
    mixSlider.setLookAndFeel(&customLookAndFeel);

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


    mixSlider.setVisible(false);



    sizeTitleLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    sizeTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sizeTitleLabel.setJustificationType(juce::Justification::centred);
    sizeTitleLabel.setText("S  I  Z  E", juce::dontSendNotification);
    addAndMakeVisible(sizeTitleLabel);

    dampTitleLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    dampTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dampTitleLabel.setJustificationType(juce::Justification::centred);
    dampTitleLabel.setText("D  E  C  A  Y", juce::dontSendNotification);
    addAndMakeVisible(dampTitleLabel);

    widthTitleLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    widthTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    widthTitleLabel.setJustificationType(juce::Justification::centred);
    widthTitleLabel.setText("W  I  D  T  H", juce::dontSendNotification);
    addAndMakeVisible(widthTitleLabel);


    sizeValueLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    sizeValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sizeValueLabel.setJustificationType(juce::Justification::centred);
    sizeValueLabel.setText(juce::String(sizeSlider.getValue(), 1), juce::dontSendNotification);
    addAndMakeVisible(sizeValueLabel);

    dampValueLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    dampValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dampValueLabel.setJustificationType(juce::Justification::centred);
    dampValueLabel.setText(juce::String(dampSlider.getValue(), 1), juce::dontSendNotification);
    addAndMakeVisible(dampValueLabel);

    widthValueLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    widthValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    widthValueLabel.setJustificationType(juce::Justification::centred);
    widthValueLabel.setText(juce::String(widthSlider.getValue(), 1), juce::dontSendNotification);
    addAndMakeVisible(widthValueLabel);

    mixValueLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    mixValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mixValueLabel.setJustificationType(juce::Justification::centred);
    mixValueLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);
    addAndMakeVisible(mixValueLabel);


    overlayBlendSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    overlayBlendSlider.setRange(0.0, 100.0, 0.1);
    overlayBlendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    overlayBlendSlider.setLookAndFeel(&customLookAndFeel);
    overlayBlendSlider.addListener(this);
    addAndMakeVisible(overlayBlendSlider);


    overlayBlendTitleLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    overlayBlendTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    overlayBlendTitleLabel.setJustificationType(juce::Justification::centred);
    overlayBlendTitleLabel.setText("F  I  L  T  E  R", juce::dontSendNotification);
    addAndMakeVisible(overlayBlendTitleLabel);

    overlayBlendValueLabel.setFont(customLookAndFeel.getcustomTypeface().withPointHeight(8.0f));
    overlayBlendValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    overlayBlendValueLabel.setJustificationType(juce::Justification::centred);
    overlayBlendValueLabel.setText(juce::String(overlayBlendSlider.getValue(), 1), juce::dontSendNotification);
    addAndMakeVisible(overlayBlendValueLabel);

    overlayBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "OVERLAY_BLEND", overlayBlendSlider);

    addAndMakeVisible(frequencyAnalyzer);
}

void EditorContent::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto iconHeight = 50;
    auto iconArea = area.removeFromTop(iconHeight);

    const int spacer = 15;
    area.removeFromTop(spacer);

    const int sliderWidthMain = 180;
    const int sliderWidthFilter = 150;
    const int sliderHeight = 14;
    const int sliderSpacing = 35;

    const int startingX = (getWidth() - sliderWidthMain) / 2;  
    int currentY = 60;

    const int labelWidth = sliderWidthMain;
    const int labelHeight = 10;
    const int labelOffset = 5;

    
    sizeTitleLabel.setBounds(startingX, currentY - labelHeight - labelOffset, labelWidth, labelHeight);
    sizeSlider.setBounds(startingX, currentY, sliderWidthMain, sliderHeight);
    sizeValueLabel.setBounds(startingX, currentY + sliderHeight + labelOffset, labelWidth, labelHeight);
    currentY += sliderHeight + sliderSpacing;

    dampTitleLabel.setBounds(startingX, currentY - labelHeight - labelOffset, labelWidth, labelHeight);
    dampSlider.setBounds(startingX, currentY, sliderWidthMain, sliderHeight);
    dampValueLabel.setBounds(startingX, currentY + sliderHeight + labelOffset, labelWidth, labelHeight);
    currentY += sliderHeight + sliderSpacing;

    widthTitleLabel.setBounds(startingX, currentY - labelHeight - labelOffset, labelWidth, labelHeight);
    widthSlider.setBounds(startingX, currentY, sliderWidthMain, sliderHeight);
    widthValueLabel.setBounds(startingX, currentY + sliderHeight + labelOffset, labelWidth, labelHeight);

    
    int filterX = (getWidth() - sliderWidthFilter) / 2;  
    int filterY = currentY + sliderSpacing + 40;

    overlayBlendTitleLabel.setBounds(filterX, filterY - labelHeight - labelOffset, sliderWidthFilter, labelHeight);
    overlayBlendSlider.setBounds(filterX, filterY, sliderWidthFilter, sliderHeight);
    overlayBlendValueLabel.setBounds(filterX, filterY + sliderHeight + labelOffset, sliderWidthFilter, labelHeight);
}

void EditorContent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sizeSlider)
    {
        float sliderValue = sizeSlider.getValue() / 100.0f;
        float sizeValue = 1.0f + (sliderValue * 1.0f);
        pluginEditor.updateCubeSize(sizeValue);
        sizeValueLabel.setText(juce::String(sizeSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &dampSlider)
    {
        float sliderValue = dampSlider.getValue() / 100.0f;
        pluginEditor.updateDampValue(sliderValue);
        dampValueLabel.setText(juce::String(dampSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &widthSlider)
    {
        float sliderValue = widthSlider.getValue() / 100.0f;
        float widthValue = sliderValue;
        pluginEditor.updateWidthValue(widthValue);
        widthValueLabel.setText(juce::String(widthSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &mixSlider)
    {
        mixValueLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &overlayBlendSlider)
    {
        overlayBlendValueLabel.setText(juce::String(overlayBlendSlider.getValue(), 1), juce::dontSendNotification);

    }
}

bool EditorContent::keyPressed(const juce::KeyPress& k)
{
    if (k.isKeyCode(juce::KeyPress::tabKey) && hasKeyboardFocus(false))
        return true;
    return false;
}

juce::Colour EditorContent::interpolateColor(const juce::Colour& startColor, const juce::Colour& endColor, float ratio)
{
    return startColor.interpolatedWith(endColor, ratio);
}