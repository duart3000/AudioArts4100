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
    class RackLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        void setKnobColour (juce::Colour c) { knobColour = c; }
        void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
        void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    private:
        juce::Colour knobColour { juce::Colour (0xffcf3030) };
    };

    AudioArts4100AudioProcessor& processor;
    RackLookAndFeel lookAndFeel;
    std::array<RackLookAndFeel, 4> bandLookAndFeels;
    RackLookAndFeel masterLookAndFeel;
    std::array<std::array<juce::Slider, 3>, 4> knobs;
    std::array<std::array<juce::Label, 3>, 4> knobLabels;
    std::array<juce::ToggleButton, 4> multiplierButtons;
    std::array<juce::ToggleButton, 4> bandBypassButtons;
    std::array<std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 3>, 4> sliderAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> multiplierAttachments, bypassAttachments;
    juce::Slider masterSlider;
    juce::Label masterLabel;
    juce::ToggleButton eqBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqBypassAttachment;
    juce::Label titleLabel, subtitleLabel, powerLabel, overloadLabel;
    juce::Colour getBandColour (int) const;
    void timerCallback() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioArts4100AudioProcessorEditor)
};
