#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AudioArts4100AudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                 private juce::Timer
{
public:
    explicit AudioArts4100AudioProcessorEditor (AudioArts4100AudioProcessor&);
    ~AudioArts4100AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class InvisibleLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
        void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
        void drawButtonText (juce::Graphics&, juce::TextButton&, bool, bool) override;
        void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    };

    AudioArts4100AudioProcessor& processor;
    InvisibleLookAndFeel invisibleLook;
    juce::Image panelBackground;

    std::array<std::array<juce::Slider, 3>, 4> knobs;
    std::array<juce::ToggleButton, 4> multiplierButtons;
    std::array<juce::ToggleButton, 4> bandBypassButtons;
    std::array<std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 3>, 4> sliderAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> multiplierAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> bypassAttachments;

    juce::Slider masterSlider;
    juce::ToggleButton eqBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqBypassAttachment;

    static juce::String parameterId (int band, const juce::String& suffix);
    void timerCallback() override;
    void drawNeedle (juce::Graphics&, juce::Slider&, float designX, float designY,
                     float designLength, float scaleX, float scaleY) const;
    void drawLed (juce::Graphics&, float designX, float designY, bool on,
                  juce::Colour onColour, float scaleX, float scaleY) const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioArts4100AudioProcessorEditor)
};
