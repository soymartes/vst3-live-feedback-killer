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
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveGateAudioProcessorEditor)
};
