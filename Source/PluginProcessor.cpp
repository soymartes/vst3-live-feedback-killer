#include "PluginProcessor.h"
#include "PluginEditor.h"

LiveGateAudioProcessor::LiveGateAudioProcessor()
     : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

LiveGateAudioProcessor::~LiveGateAudioProcessor() {}

const juce::String LiveGateAudioProcessor::getName() const { return "LiveGate"; }
bool LiveGateAudioProcessor::acceptsMidi() const { return false; }
bool LiveGateAudioProcessor::producesMidi() const { return false; }
bool LiveGateAudioProcessor::isMidiEffect() const { return false; }
double LiveGateAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int LiveGateAudioProcessor::getNumPrograms() { return 1; }
int LiveGateAudioProcessor::getCurrentProgram() { return 0; }
void LiveGateAudioProcessor::setCurrentProgram(int index) {}
const juce::String LiveGateAudioProcessor::getProgramName(int index) { return {}; }
void LiveGateAudioProcessor::changeProgramName(int index, const juce::String& newName) {}

juce::AudioProcessorValueTreeState::ParameterLayout LiveGateAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", -60.0f, 0.0f, -24.0f));
    return { params.begin(), params.end() };
}

void LiveGateAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void LiveGateAudioProcessor::releaseResources() {}

bool LiveGateAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

void LiveGateAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float thresholdDB = apvts.getRawParameterValue("threshold")->load();
    float thresholdVal = juce::Decibels::decibelsToGain(thresholdDB);

    auto numSamples = buffer.getNumSamples();
    float rms = buffer.getRMSLevel(0, 0, numSamples);

    float targetGain = (rms >= thresholdVal) ? 1.0f : 0.0f;

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample) {
            currentGain = juce::jmap(0.05f, currentGain, targetGain); // Suavizado básico
            channelData[sample] *= currentGain;
        }
    }
}

bool LiveGateAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* LiveGateAudioProcessor::createEditor() { return new LiveGateAudioProcessorEditor(*this); }

void LiveGateAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {}
void LiveGateAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginProcessor() {
    return new LiveGateAudioProcessor();
}
