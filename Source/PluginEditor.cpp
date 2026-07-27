#include "PluginProcessor.h"
#include "PluginEditor.h"

LiveGateAudioProcessorEditor::LiveGateAudioProcessorEditor(LiveGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(thresholdSlider);

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "threshold", thresholdSlider);

    setSize(300, 300);
}

LiveGateAudioProcessorEditor::~LiveGateAudioProcessorEditor() {}

void LiveGateAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Live Gate Test", getLocalBounds().removeFromTop(40), juce::Justification::centred, 1);
}

void LiveGateAudioProcessorEditor::resized() {
    thresholdSlider.setBounds(75, 75, 150, 150);
}
