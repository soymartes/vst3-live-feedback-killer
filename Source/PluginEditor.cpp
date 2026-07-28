#include "PluginProcessor.h"
#include "PluginEditor.h"

LiveGateAudioProcessorEditor::LiveGateAudioProcessorEditor(LiveGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    auto configureSliderAndLabel = [this](juce::Slider& slider, juce::Label& label, const juce::String& text) {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, false);
        label.setFont(12.0f);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    configureSliderAndLabel(thresholdSlider, thresholdLabel, "Threshold");
    configureSliderAndLabel(attackSlider, attackLabel, "Attack");
    configureSliderAndLabel(releaseSlider, releaseLabel, "Release");
    configureSliderAndLabel(rangeSlider, rangeLabel, "Range");
    configureSliderAndLabel(notchFreqSlider, notchFreqLabel, "Notch Freq");

    fbButton.setButtonText("Enable Feedback Killer");
    addAndMakeVisible(fbButton);

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "threshold", thresholdSlider);
    attackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    releaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    rangeAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "range", rangeSlider);
    notchFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fb_freq", notchFreqSlider);
    fbButtonAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fb_enable", fbButton);

    setSize(520, 300);
}

LiveGateAudioProcessorEditor::~LiveGateAudioProcessorEditor() {}

void LiveGateAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText("Live Gate + Notch Filter", getLocalBounds().removeFromTop(35), juce::Justification::centred, 1);
}

void LiveGateAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(35);
    
    auto sliderArea = bounds.removeFromTop(200);
    int sliderWidth = sliderArea.getWidth() / 5;
    
    auto layoutSliderWithMargin = [](juce::Rectangle<int> boundsRect) {
        return boundsRect.reduced(5, 18); 
    };

    thresholdSlider.setBounds(layoutSliderWithMargin(sliderArea.removeFromLeft(sliderWidth)));
    attackSlider.setBounds(layoutSliderWithMargin(sliderArea.removeFromLeft(sliderWidth)));
    releaseSlider.setBounds(layoutSliderWithMargin(sliderArea.removeFromLeft(sliderWidth)));
    rangeSlider.setBounds(layoutSliderWithMargin(sliderArea.removeFromLeft(sliderWidth)));
    notchFreqSlider.setBounds(layoutSliderWithMargin(sliderArea));

    fbButton.setBounds(bounds.reduced(100, 5));
}
