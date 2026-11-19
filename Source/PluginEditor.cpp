#include "PluginEditor.h"

namespace
{
    const std::array<juce::Colour, 4> bandColours {
        juce::Colour (0xffe33b35), juce::Colour (0xffed842d),
        juce::Colour (0xffe7c83e), juce::Colour (0xff4eae69)
    };
    const std::array<juce::String, 4> bandNames { "BAND 1  /  LOW", "BAND 2  /  LOW MID", "BAND 3  /  HIGH MID", "BAND 4  /  HIGH" };
    const std::array<juce::String, 3> controlNames { "FREQUENCY", "BANDWIDTH", "BOOST / CUT" };
    juce::String parameterId (int band, const juce::String& suffix)
    {
        return "b" + juce::String (band + 1) + "_" + suffix;
    }
}

AudioArts4100AudioProcessorEditor::AudioArts4100AudioProcessorEditor (AudioArts4100AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (980, 650);

    titleLabel.setText ("AUDIOARTS", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (27.0f, juce::Font::bold)));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe7e4dc));
    addAndMakeVisible (titleLabel);
    subtitleLabel.setText ("MODEL 4100   /   FOUR BAND PARAMETRIC EQUALIZER - PREAMP", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff999b9d));
    addAndMakeVisible (subtitleLabel);

    for (int i = 0; i < 4; ++i)
    {
        lookAndFeel.setKnobColour (getBandColour (i));
        for (int j = 0; j < 3; ++j)
        {
            auto& slider = knobs[(size_t)i][(size_t)j];
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 82, 20);
            if (j == 0)
            {
                slider.setTextFromValueFunction ([this, i](double v)
                {
                    const bool x10 = processor.parameters.getRawParameterValue (parameterId (i, "x10"))->load() >= 0.5f;
                    return juce::String (v * (x10 ? 10.0 : 1.0), 0) + " Hz";
                });
            }
            else if (j == 1) slider.setTextValueSuffix (" oct");
            else slider.setTextValueSuffix (" dB");
            slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe5e2db));
            slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff17191b));
            slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff34383b));
            slider.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (slider);
            auto& label = knobLabels[(size_t)i][(size_t)j];
            label.setText (controlNames[(size_t)j], juce::dontSendNotification);
            label.setJustificationType (juce::Justification::centred);
            label.setFont (juce::Font (juce::FontOptions (9.5f, juce::Font::bold)));
            label.setColour (juce::Label::textColourId, juce::Colour (0xffb8b8b4));
            addAndMakeVisible (label);
        }
        sliderAttachments[(size_t)i][0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "freq"), knobs[(size_t)i][0]);
        sliderAttachments[(size_t)i][1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "width"), knobs[(size_t)i][1]);
        sliderAttachments[(size_t)i][2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "gain"), knobs[(size_t)i][2]);

        multiplierButtons[(size_t)i].setButtonText ("FREQ ×10");
        multiplierButtons[(size_t)i].setClickingTogglesState (true);
        multiplierButtons[(size_t)i].setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd9d7d0));
        addAndMakeVisible (multiplierButtons[(size_t)i]);
        multiplierAttachments[(size_t)i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, parameterId (i, "x10"), multiplierButtons[(size_t)i]);

        bandBypassButtons[(size_t)i].setButtonText ("BAND BYPASS");
        bandBypassButtons[(size_t)i].setClickingTogglesState (true);
        bandBypassButtons[(size_t)i].setToggleState (true, juce::dontSendNotification);
        bandBypassButtons[(size_t)i].setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd9d7d0));
        addAndMakeVisible (bandBypassButtons[(size_t)i]);
        bypassAttachments[(size_t)i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, parameterId (i, "bypass"), bandBypassButtons[(size_t)i]);
    }

    masterSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    masterSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 82, 20);
    masterSlider.setTextValueSuffix (" dB");
    masterSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe5e2db));
    masterSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff17191b));
    masterSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff34383b));
    lookAndFeel.setKnobColour (juce::Colour (0xff3996d8));
    masterSlider.setLookAndFeel (&lookAndFeel);
    addAndMakeVisible (masterSlider);
    masterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, "master", masterSlider);
    masterLabel.setText ("MASTER LEVEL", juce::dontSendNotification);
    masterLabel.setJustificationType (juce::Justification::centred);
    masterLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    masterLabel.setColour (juce::Label::textColourId, juce::Colour (0xffc8c8c3));
    addAndMakeVisible (masterLabel);

    eqBypassButton.setButtonText ("EQ BYPASS");
    eqBypassButton.setClickingTogglesState (true);
    eqBypassButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffe6e3dc));
    addAndMakeVisible (eqBypassButton);
    eqBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, "eq_bypass", eqBypassButton);

    powerLabel.setText ("●  POWER", juce::dontSendNotification);
    powerLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    powerLabel.setColour (juce::Label::textColourId, juce::Colour (0xff73c879));
    addAndMakeVisible (powerLabel);
    overloadLabel.setText ("●  OVERLOAD", juce::dontSendNotification);
    overloadLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    overloadLabel.setColour (juce::Label::textColourId, juce::Colour (0xff777777));
    addAndMakeVisible (overloadLabel);
    startTimerHz (15);
}

AudioArts4100AudioProcessorEditor::~AudioArts4100AudioProcessorEditor()
{
    stopTimer();
    masterSlider.setLookAndFeel (nullptr);
    for (auto& row : knobs) for (auto& knob : row) knob.setLookAndFeel (nullptr);
    setLookAndFeel (nullptr);
}

juce::Colour AudioArts4100AudioProcessorEditor::getBandColour (int i) const
{
    return bandColours[(size_t)juce::jlimit (0, 3, i)];
}

void AudioArts4100AudioProcessorEditor::RackLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);
    const float diameter = (float) juce::jmin (width - 18, height - 27);
    const float cx = (float)x + (float)width * 0.5f;
    const float cy = (float)y + diameter * 0.5f + 3.0f;
    const float r = diameter * 0.5f;
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    // Outer machined bezel and recessed shadow.
    g.setColour (juce::Colour (0xff08090a));
    g.fillEllipse (cx - r - 4.0f, cy - r - 2.0f, (r + 4.0f) * 2.0f, (r + 4.0f) * 2.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff777b7d), cx - r, cy - r,
                                             juce::Colour (0xff202326), cx + r, cy + r, false));
    g.fillEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.setColour (juce::Colour (0xff111315));
    g.fillEllipse (cx - r + 3.0f, cy - r + 3.0f, (r - 3.0f) * 2.0f, (r - 3.0f) * 2.0f);

    // Calibrated scale ticks.
    const float tickR1 = r + 7.0f, tickR2 = r + 12.0f;
    for (int i = 0; i <= 20; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * (float)i / 20.0f;
        const bool major = (i % 5 == 0);
        juce::Point<float> p1 (cx + std::cos (a) * (major ? tickR1 - 2.0f : tickR1), cy + std::sin (a) * (major ? tickR1 - 2.0f : tickR1));
        juce::Point<float> p2 (cx + std::cos (a) * tickR2, cy + std::sin (a) * tickR2);
        g.setColour (major ? juce::Colour (0xffd2d0c9) : juce::Colour (0xff73777a));
        g.drawLine ({ p1, p2 }, major ? 1.5f : 1.0f);
    }

    // Moulded plastic cap, with a subtle highlight and lower-edge shading.
    auto cap = knobColour;
    g.setGradientFill (juce::ColourGradient (cap.brighter (0.42f), cx - r * 0.55f, cy - r * 0.65f,
                                             cap.darker (0.48f), cx + r * 0.65f, cy + r * 0.75f, false));
    g.fillEllipse (cx - r * 0.78f, cy - r * 0.78f, r * 1.56f, r * 1.56f);
    g.setColour (cap.darker (0.65f));
    g.drawEllipse (cx - r * 0.78f, cy - r * 0.78f, r * 1.56f, r * 1.56f, 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.16f));
    g.drawArc (cx - r * 0.69f, cy - r * 0.69f, r * 1.38f, r * 1.38f, 3.7f, 5.25f, 2.0f);

    // Pointer inset into the plastic, plus a small centre boss.
    const float pointerLength = r * 0.56f;
    juce::Point<float> pointerEnd (cx + std::sin (angle) * pointerLength, cy - std::cos (angle) * pointerLength);
    g.setColour (juce::Colour (0xff17191b));
    g.drawLine (cx, cy, pointerEnd.x, pointerEnd.y, 5.0f);
    g.setColour (juce::Colour (0xfff1eee5));
    g.drawLine (cx, cy, pointerEnd.x, pointerEnd.y, 1.5f);
    g.setColour (cap.brighter (0.15f));
    g.fillEllipse (cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
}

void AudioArts4100AudioProcessorEditor::RackLookAndFeel::drawToggleButton (juce::Graphics& g,
    juce::ToggleButton& button, bool highlighted, bool down)
{
    auto r = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();
    g.setColour (juce::Colour (0xff08090a));
    g.fillRoundedRectangle (r.translated (0.0f, 1.5f), 4.0f);
    g.setGradientFill (juce::ColourGradient (on ? juce::Colour (0xff555b5e) : juce::Colour (0xff34383b),
        r.getX(), r.getY(), juce::Colour (0xff171a1c), r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (on ? juce::Colour (0xff8bc98b) : juce::Colour (0xff55595b));
    g.fillEllipse (r.getX() + 7.0f, r.getCentreY() - 3.0f, 6.0f, 6.0f);
    g.setColour (button.findColour (juce::ToggleButton::textColourId).withAlpha (highlighted || down ? 1.0f : 0.88f));
    g.setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
    g.drawFittedText (button.getButtonText(), button.getLocalBounds().withTrimmedLeft (17), juce::Justification::centred, 1);
}

void AudioArts4100AudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff080a0c), 0.0f, 0.0f,
                                             juce::Colour (0xff171a1d), 0.0f, bounds.getBottom(), false));
    g.fillAll();
    auto panel = bounds.reduced (12.0f);
    g.setColour (juce::Colour (0xff050607));
    g.fillRoundedRectangle (panel, 8.0f);
    g.setColour (juce::Colour (0xff41464a));
    g.drawRoundedRectangle (panel, 8.0f, 1.0f);
    // Brushed-metal horizontal hairlines, kept very subtle against the near-black panel.
    g.setColour (juce::Colours::white.withAlpha (0.018f));
    for (int y = 16; y < getHeight() - 14; y += 4) g.drawHorizontalLine (y, 14.0f, (float)getWidth() - 14.0f);

    // Four and only four rack screws, one in each corner.
    for (auto pt : { juce::Point<float> (25.0f, 25.0f), juce::Point<float> ((float)getWidth() - 25.0f, 25.0f),
                     juce::Point<float> (25.0f, (float)getHeight() - 25.0f), juce::Point<float> ((float)getWidth() - 25.0f, (float)getHeight() - 25.0f) })
    {
        g.setColour (juce::Colour (0xff050607)); g.fillEllipse (pt.x - 7.0f, pt.y - 7.0f, 14.0f, 14.0f);
        g.setColour (juce::Colour (0xff74797c)); g.drawEllipse (pt.x - 6.0f, pt.y - 6.0f, 12.0f, 12.0f, 1.0f);
        g.setColour (juce::Colour (0xff8d9294)); g.drawLine (pt.x - 3.0f, pt.y + 3.0f, pt.x + 3.0f, pt.y - 3.0f, 1.2f);
    }

    for (int i = 0; i < 4; ++i)
    {
        auto area = (i == 0 ? juce::Rectangle<float> (28, 94, 452, 224) : i == 1 ? juce::Rectangle<float> (500, 94, 452, 224)
                    : i == 2 ? juce::Rectangle<float> (28, 330, 452, 224) : juce::Rectangle<float> (500, 330, 452, 224));
        g.setColour (juce::Colour (0xff111416)); g.fillRoundedRectangle (area, 5.0f);
        g.setColour (juce::Colour (0xff41474b)); g.drawRoundedRectangle (area, 5.0f, 1.0f);
        g.setColour (getBandColour (i)); g.fillRoundedRectangle (area.getX() + 1.0f, area.getY() + 1.0f, 5.0f, area.getHeight() - 2.0f, 2.0f);
        g.setColour (getBandColour (i).brighter (0.15f));
        g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
        g.drawText (bandNames[(size_t)i], area.toNearestInt().withTrimmedLeft (16).withHeight (22).translated (0, 5), juce::Justification::centredLeft);
        g.setColour (juce::Colour (0xff44494c));
        g.drawHorizontalLine ((int)area.getY() + 32, area.getX() + 14.0f, area.getRight() - 12.0f);
    }
    g.setColour (juce::Colour (0xff41474b));
    g.drawHorizontalLine (566, 28.0f, (float)getWidth() - 28.0f);
}

void AudioArts4100AudioProcessorEditor::resized()
{
    titleLabel.setBounds (48, 28, 270, 34);
    subtitleLabel.setBounds (325, 37, 550, 20);
    const std::array<juce::Point<int>, 4> origins { juce::Point<int> (42, 137), {514, 137}, {42, 373}, {514, 373} };
    for (int i = 0; i < 4; ++i)
    {
        const auto origin = origins[(size_t)i];
        for (int j = 0; j < 3; ++j)
        {
            const int x = origin.x + 15 + j * 139;
            knobLabels[(size_t)i][(size_t)j].setBounds (x, origin.y - 3, 126, 16);
            knobs[(size_t)i][(size_t)j].setBounds (x + 8, origin.y + 12, 110, 137);
        }
        multiplierButtons[(size_t)i].setBounds (origin.x + 26, origin.y + 158, 112, 23);
        bandBypassButtons[(size_t)i].setBounds (origin.x + 274, origin.y + 158, 112, 23);
    }
    masterSlider.setBounds (410, 563, 150, 83);
    masterLabel.setBounds (410, 553, 150, 15);
    eqBypassButton.setBounds (585, 592, 125, 25);
    powerLabel.setBounds (740, 592, 90, 24);
    overloadLabel.setBounds (835, 592, 110, 24);
}

void AudioArts4100AudioProcessorEditor::timerCallback()
{
    for (int i = 0; i < 4; ++i)
        knobs[(size_t)i][0].setValue (knobs[(size_t)i][0].getValue(), juce::dontSendNotification);
    const bool hot = processor.getOutputPeak() >= 0.98f;
    overloadLabel.setColour (juce::Label::textColourId, hot ? juce::Colour (0xffff5148) : juce::Colour (0xff777777));
}
