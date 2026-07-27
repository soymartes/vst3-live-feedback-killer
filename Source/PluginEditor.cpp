#include "PluginProcessor.h"
#include "PluginEditor.h"

LiveGateAudioProcessorEditor::LiveGateAudioProcessorEditor(LiveGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    auto configureSlider = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(slider);
    };

    configureSlider(thresholdSlider);
    configureSlider(attackSlider);
    configureSlider(releaseSlider);
    configureSlider(rangeSlider);

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "threshold", thresholdSlider);
    attackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    releaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    rangeAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "range", rangeSlider);

    setSize(400, 250);
}

LiveGateAudioProcessorEditor::~LiveGateAudioProcessorEditor() {}

void LiveGateAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText("Live Gate - DSP Refinado", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
}

void LiveGateAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(30); // Espacio para el título
    
    int sliderWidth = bounds.getWidth() / 4;
    thresholdSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(5));
    attackSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(5));
    releaseSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(5));
    rangeSlider.setBounds(bounds.reduced(5));
}
