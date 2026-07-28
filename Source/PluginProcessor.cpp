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
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack (ms)", 0.1f, 100.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release (ms)", 10.0f, 1000.0f, 200.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("range", "Range (dB)", -60.0f, 0.0f, -40.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("fb_enable", "Feedback Killer", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fb_freq", "Notch Freq", juce::NormalisableRange<float>(100.0f, 8000.0f, 1.0f, 0.4f), 1000.0f));
    return { params.begin(), params.end() };
}

void LiveGateAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    currentSampleRate = sampleRate;
    envelope = 0.0f;
    currentGainDb = 0.0f;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32>(samplesPerBlock);
    spec.numChannels = static_cast<uint32>(getTotalNumInputChannels());
    
    notchFilter.prepare(spec);
    *notchFilter.state = *juce::dsp::IIR::Coefficients<float>::makeNotchFilter(sampleRate, 1000.0f);
}

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

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float thresholdDB = apvts.getRawParameterValue("threshold")->load();
    float attackMs    = apvts.getRawParameterValue("attack")->load();
    float releaseMs   = apvts.getRawParameterValue("release")->load();
    float rangeDB     = apvts.getRawParameterValue("range")->load();
    bool fbEnabled    = apvts.getRawParameterValue("fb_enable")->load() > 0.5f;
    float fbFreq      = apvts.getRawParameterValue("fb_freq")->load();

    // 1. Aplicar Filtro Notch si está activado
    if (fbEnabled) {
        *notchFilter.state = *juce::dsp::IIR::Coefficients<float>::makeNotchFilter(currentSampleRate, fbFreq);
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        notchFilter.process(context);
    }

    // 2. Procesar Noise Gate (envolvente por muestra)
    float attackCoeff  = std::exp(-1.0f / (static_cast<float>(currentSampleRate) * attackMs * 0.001f));
    float releaseCoeff = std::exp(-1.0f / (static_cast<float>(currentSampleRate) * releaseMs * 0.001f));

    auto numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample) {
        float inputSample = 0.0f;
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            inputSample = std::max(inputSample, std::abs(buffer.getReadPointer(channel)[sample]));
        }

        float inputDb = (inputSample > 0.00001f) ? juce::Decibels::gainToDecibels(inputSample) : -100.0f;
        
        if (inputDb > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * inputDb;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * inputDb;

        float targetGainDb = 0.0f;
        if (envelope < thresholdDB) {
            float diff = thresholdDB - envelope;
            targetGainDb = -diff;
            if (targetGainDb < rangeDB)
                targetGainDb = rangeDB;
        }

        currentGainDb = 0.99f * currentGainDb + 0.01f * targetGainDb;
        float linearGain = juce::Decibels::decibelsToGain(currentGainDb);

        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            buffer.getWritePointer(channel)[sample] *= linearGain;
        }
    }
}

bool LiveGateAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* LiveGateAudioProcessor::createEditor() { return new LiveGateAudioProcessorEditor(*this); }

void LiveGateAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {}
void LiveGateAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new LiveGateAudioProcessor();
}
