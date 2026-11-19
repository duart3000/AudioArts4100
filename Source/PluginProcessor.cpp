#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr std::array<float, 4> minFreq { 22.0f, 22.0f, 100.0f, 180.0f };
    constexpr std::array<float, 4> maxFreq { 300.0f, 300.0f, 1000.0f, 2100.0f };
    juce::String bandId (int band, const juce::String& suffix)
    {
        return "b" + juce::String (band + 1) + "_" + suffix;
    }
}

AudioArts4100AudioProcessor::AudioArts4100AudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioArts4100AudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    for (int i = 0; i < 4; ++i)
    {
        const auto id = [i](const String& s) { return bandId (i, s); };
        p.push_back (std::make_unique<AudioParameterFloat> (id ("freq"), "Band " + String (i + 1) + " Frequency",
            NormalisableRange<float> (minFreq[(size_t)i], maxFreq[(size_t)i], 0.01f, 0.35f),
            (minFreq[(size_t)i] + maxFreq[(size_t)i]) * 0.5f));
        p.push_back (std::make_unique<AudioParameterFloat> (id ("width"), "Band " + String (i + 1) + " Bandwidth",
            NormalisableRange<float> (1.0f / 6.0f, 2.0f, 0.001f), 1.0f));
        p.push_back (std::make_unique<AudioParameterFloat> (id ("gain"), "Band " + String (i + 1) + " Gain",
            NormalisableRange<float> (-16.0f, 16.0f, 0.01f), 0.0f));
        p.push_back (std::make_unique<AudioParameterBool> (id ("x10"), "Band " + String (i + 1) + " x10", false));
        p.push_back (std::make_unique<AudioParameterBool> (id ("bypass"), "Band " + String (i + 1) + " Bypass", false));
    }
    p.push_back (std::make_unique<AudioParameterFloat> ("master", "Master Level",
        NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
    p.push_back (std::make_unique<AudioParameterBool> ("eq_bypass", "EQ Bypass", false));
    return { p.begin(), p.end() };
}

void AudioArts4100AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 1 };
    juce::dsp::ProcessSpec gainSpec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) getTotalNumOutputChannels() };
    for (auto& b : bands)
    {
        b.left.reset(); b.right.reset();
        b.left.prepare (spec); b.right.prepare (spec);
        b.lastFrequency = -1.0f;
    }
    masterGain.prepare (gainSpec);
    masterGain.setRampDurationSeconds (0.02);
    updateFilters();
}

void AudioArts4100AudioProcessor::releaseResources() {}

bool AudioArts4100AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    if (in != out) return false;
    return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
}

void AudioArts4100AudioProcessor::updateFilters()
{
    for (int i = 0; i < 4; ++i)
    {
        auto& b = bands[(size_t)i];
        const auto value = [this, i](const juce::String& s) { return parameters.getRawParameterValue (bandId (i, s))->load(); };
        const float multiplier = value ("x10") >= 0.5f ? 10.0f : 1.0f;
        const float freq = juce::jlimit (10.0f, (float) (currentSampleRate * 0.45), value ("freq") * multiplier);
        const float bw = value ("width");
        const float gain = value ("gain");
        const bool enabled = value ("bypass") < 0.5f;
        if (freq != b.lastFrequency || bw != b.lastBandwidth || gain != b.lastGain || enabled != b.lastEnabled)
        {
            const float w0 = juce::MathConstants<float>::twoPi * freq / (float) currentSampleRate;
            const float sinW = std::max (0.0001f, std::sin (w0));
            const float q = sinW / (2.0f * std::sinh (std::log (2.0f) * 0.5f * bw * w0 / sinW));
            b.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, freq,
                juce::jlimit (0.05f, 20.0f, q), juce::Decibels::decibelsToGain (gain));
            b.left.coefficients = b.coefficients;
            b.right.coefficients = b.coefficients;
            b.lastFrequency = freq; b.lastBandwidth = bw; b.lastGain = gain; b.lastEnabled = enabled;
        }
    }
}

void AudioArts4100AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = getTotalNumInputChannels();
    for (int ch = channels; ch < getTotalNumOutputChannels(); ++ch) buffer.clear (ch, 0, buffer.getNumSamples());
    updateFilters();
    const bool eqBypass = parameters.getRawParameterValue ("eq_bypass")->load() >= 0.5f;
    const float masterDb = parameters.getRawParameterValue ("master")->load();
    masterGain.setGainDecibels (masterDb);

    if (! eqBypass)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (bands[(size_t)i].lastEnabled)
            {
                auto& b = bands[(size_t)i];
                for (int ch = 0; ch < channels; ++ch)
                {
                    juce::dsp::AudioBlock<float> block (buffer);
                    auto single = block.getSingleChannelBlock ((size_t) ch);
                    juce::dsp::ProcessContextReplacing<float> context (single);
                    if (ch == 0) b.left.process (context); else b.right.process (context);
                }
            }
        }
    }
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    masterGain.process (context);
    float peak = 0.0f;
    for (int ch = 0; ch < channels; ++ch)
        peak = std::max (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
    outputPeak.store (peak);
}

juce::AudioProcessorEditor* AudioArts4100AudioProcessor::createEditor()
{
    return new AudioArts4100AudioProcessorEditor (*this);
}

void AudioArts4100AudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void AudioArts4100AudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioArts4100AudioProcessor();
}
