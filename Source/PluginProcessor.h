#pragma once
#include <JuceHeader.h>

class AudioArts4100AudioProcessor final : public juce::AudioProcessor
{
public:
    AudioArts4100AudioProcessor();
    ~AudioArts4100AudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "AudioArts 4100"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    float getOutputPeak() const noexcept { return outputPeak.load(); }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct Band
    {
        juce::dsp::IIR::Filter<float> left, right;
        juce::dsp::IIR::Coefficients<float>::Ptr coefficients;
        float lastFrequency = -1.0f, lastBandwidth = -1.0f, lastGain = 999.0f;
        bool lastEnabled = true;
    };
    std::array<Band, 4> bands;
    juce::dsp::Gain<float> masterGain;
    double currentSampleRate = 44100.0;
    std::atomic<float> outputPeak { 0.0f };
    void updateFilters();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioArts4100AudioProcessor)
};
