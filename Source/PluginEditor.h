#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class LiveGateAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    LiveGateAudioProcessorEditor(LiveGateAudioProcessor&);
    ~LiveGateAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LiveGateAudioProcessor& audioProcessor;

    juce::Slider thresholdSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider rangeSlider;

    juce::Label thresholdLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label rangeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveGateAudioProcessorEditor)
};
