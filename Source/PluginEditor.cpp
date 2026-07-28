#include "PluginProcessor.h"
#include "PluginEditor.h"

LiveGateAudioProcessorEditor::LiveGateAudioProcessorEditor(LiveGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    auto configureSliderAndLabel = [this](juce::Slider& slider, juce::Label& label, const juce::String& text) {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, false); // Asocia la etiqueta en la parte superior del slider
        label.setFont(12.0f);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    configureSliderAndLabel(thresholdSlider, thresholdLabel, "Threshold");
    configureSliderAndLabel(attackSlider, attackLabel, "Attack");
    configureSliderAndLabel(releaseSlider, releaseLabel, "Release");
    configureSliderAndLabel(rangeSlider, rangeLabel, "Range");

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "threshold", thresholdSlider);
    attackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    releaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    rangeAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "range", rangeSlider);

    setSize(400, 280);
}

LiveGateAudioProcessorEditor::~LiveGateAudioProcessorEditor() {}

void LiveGateAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText("Live Gate - DSP Refinado", getLocalBounds().removeFromTop(35), juce::Justification::centred, 1);
}

void LiveGateAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(35); // Margen para el título principal
    
    int sliderWidth = bounds.getWidth() / 4;
    
    // Margen interno para evitar superposición con las etiquetas superiores
    auto layoutSliderWithMargin = [](juce::Rectangle<int> boundsRect) {
        return boundsRect.reduced(5, 18); 
    };

    thresholdSlider.setBounds(layoutSliderWithMargin(bounds.removeFromLeft(sliderWidth)));
    attackSlider.setBounds(layoutSliderWithMargin(bounds.removeFromLeft(sliderWidth)));
    releaseSlider.setBounds(layoutSliderWithMargin(bounds.removeFromLeft(sliderWidth)));
    rangeSlider.setBounds(layoutSliderWithMargin(bounds));
}
