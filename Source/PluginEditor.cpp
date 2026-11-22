#include "PluginEditor.h"

namespace
{
    const std::array<juce::Colour, 4> bandColours {
        juce::Colour (0xffc92f29), juce::Colour (0xffe87519),
        juce::Colour (0xffe5bd25), juce::Colour (0xff4caa45)
    };
    const std::array<juce::String, 4> bandNames {
        "BAND 1  —  LOW FREQUENCY", "BAND 2  —  LOW MID FREQUENCY",
        "BAND 3  —  HIGH MID FREQUENCY", "BAND 4  —  HIGH FREQUENCY"
    };
    const std::array<juce::String, 3> controlNames { "FREQUENCY", "OCTAVES", "dB" };
    juce::String parameterId (int band, const juce::String& suffix)
    {
        return "b" + juce::String (band + 1) + "_" + suffix;
    }
}

AudioArts4100AudioProcessorEditor::AudioArts4100AudioProcessorEditor (AudioArts4100AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (1200, 800);

    titleLabel.setText ("AUDIOARTS   |   MODEL 4100", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (29.0f, juce::Font::bold)));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe7e2d6));
    addAndMakeVisible (titleLabel);
    subtitleLabel.setText ("FOUR BAND PARAMETRIC EQUALIZER / PREAMPLIFIER", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colour (0xffc0b9a9));
    addAndMakeVisible (subtitleLabel);

    for (int i = 0; i < 4; ++i)
    {
        bandLookAndFeels[(size_t)i].setKnobColour (getBandColour (i));
        for (int j = 0; j < 3; ++j)
        {
            auto& slider = knobs[(size_t)i][(size_t)j];
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            slider.setPopupDisplayEnabled (true, false, this);
            slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe5e2db));
            slider.setLookAndFeel (&bandLookAndFeels[(size_t)i]);
            addAndMakeVisible (slider);

            auto& label = knobLabels[(size_t)i][(size_t)j];
            label.setText (controlNames[(size_t)j], juce::dontSendNotification);
            label.setJustificationType (juce::Justification::centred);
            label.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
            label.setColour (juce::Label::textColourId, juce::Colour (0xffd0c8b8));
            addAndMakeVisible (label);
        }

        sliderAttachments[(size_t)i][0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "freq"), knobs[(size_t)i][0]);
        sliderAttachments[(size_t)i][1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "width"), knobs[(size_t)i][1]);
        sliderAttachments[(size_t)i][2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, parameterId (i, "gain"), knobs[(size_t)i][2]);

        multiplierButtons[(size_t)i].setButtonText ("×10");
        multiplierButtons[(size_t)i].setClickingTogglesState (true);
        multiplierButtons[(size_t)i].setColour (juce::ToggleButton::textColourId, juce::Colour (0xffe6dfd0));
        addAndMakeVisible (multiplierButtons[(size_t)i]);
        multiplierAttachments[(size_t)i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, parameterId (i, "x10"), multiplierButtons[(size_t)i]);

        bandBypassButtons[(size_t)i].setButtonText ("BYPASS");
        bandBypassButtons[(size_t)i].setClickingTogglesState (true);
        bandBypassButtons[(size_t)i].setColour (juce::ToggleButton::textColourId, juce::Colour (0xffe6dfd0));
        addAndMakeVisible (bandBypassButtons[(size_t)i]);
        bypassAttachments[(size_t)i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, parameterId (i, "bypass"), bandBypassButtons[(size_t)i]);
    }

    masterSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    masterSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    masterSlider.setPopupDisplayEnabled (true, false, this);
    masterLookAndFeel.setKnobColour (juce::Colour (0xff1674b8));
    masterSlider.setLookAndFeel (&masterLookAndFeel);
    addAndMakeVisible (masterSlider);
    masterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, "master", masterSlider);
    masterLabel.setText ("MASTER LEVEL", juce::dontSendNotification);
    masterLabel.setJustificationType (juce::Justification::centred);
    masterLabel.setFont (juce::Font (juce::FontOptions (17.0f, juce::Font::bold)));
    masterLabel.setColour (juce::Label::textColourId, juce::Colour (0xffd0c8b8));
    addAndMakeVisible (masterLabel);

    eqBypassButton.setButtonText ("EQ BYPASS");
    eqBypassButton.setClickingTogglesState (true);
    eqBypassButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffe6dfd0));
    addAndMakeVisible (eqBypassButton);
    eqBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, "eq_bypass", eqBypassButton);

    powerLabel.setText ("POWER", juce::dontSendNotification);
    powerLabel.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    powerLabel.setColour (juce::Label::textColourId, juce::Colour (0xffd0c8b8));
    powerLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (powerLabel);
    overloadLabel.setText ("OVERLOAD", juce::dontSendNotification);
    overloadLabel.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    overloadLabel.setColour (juce::Label::textColourId, juce::Colour (0xffd0c8b8));
    overloadLabel.setJustificationType (juce::Justification::centred);
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
    const float diameter = (float)juce::jmin (width - 24, height - 28);
    const float cx = (float)x + (float)width * 0.5f;
    const float cy = (float)y + (float)height * 0.48f;
    const float r = diameter * 0.5f;
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    g.setColour (juce::Colours::black.withAlpha (0.8f));
    g.fillEllipse (cx - r - 5.0f, cy - r - 3.0f, (r + 5.0f) * 2.0f, (r + 5.0f) * 2.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffaaa69b), cx - r, cy - r,
                                             juce::Colour (0xff252729), cx + r, cy + r, false));
    g.fillEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.setColour (juce::Colour (0xff080a0b));
    g.fillEllipse (cx - r + 4.0f, cy - r + 4.0f, (r - 4.0f) * 2.0f, (r - 4.0f) * 2.0f);

    for (int i = 0; i <= 20; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * (float)i / 20.0f;
        const bool major = (i % 5 == 0);
        const float r1 = r + (major ? 7.0f : 9.0f);
        const float r2 = r + 17.0f;
        g.setColour (major ? juce::Colour (0xffe3ded2) : juce::Colour (0xff77736b));
        g.drawLine (cx + std::cos (a) * r1, cy + std::sin (a) * r1,
                    cx + std::cos (a) * r2, cy + std::sin (a) * r2, major ? 2.0f : 1.2f);
    }

    auto cap = knobColour;
    g.setGradientFill (juce::ColourGradient (cap.brighter (0.48f), cx - r * 0.6f, cy - r * 0.7f,
                                             cap.darker (0.52f), cx + r * 0.65f, cy + r * 0.72f, false));
    g.fillEllipse (cx - r * 0.78f, cy - r * 0.78f, r * 1.56f, r * 1.56f);
    g.setColour (cap.darker (0.7f));
    g.drawEllipse (cx - r * 0.78f, cy - r * 0.78f, r * 1.56f, r * 1.56f, 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.2f));
    juce::Path gloss;
    gloss.addArc (cx - r * 0.68f, cy - r * 0.68f, r * 1.36f, r * 1.36f, 3.65f, 5.15f, true);
    g.strokePath (gloss, juce::PathStrokeType (2.0f));

    const juce::Point<float> tip (cx + std::sin (angle) * r * 0.57f, cy - std::cos (angle) * r * 0.57f);
    g.setColour (juce::Colour (0xff151515));
    g.drawLine (cx + 1.0f, cy + 1.0f, tip.x + 1.0f, tip.y + 1.0f, 6.0f);
    g.setColour (juce::Colour (0xfff2eee4));
    g.drawLine (cx, cy, tip.x, tip.y, 2.0f);
}

void AudioArts4100AudioProcessorEditor::RackLookAndFeel::drawToggleButton (juce::Graphics& g,
    juce::ToggleButton& button, bool highlighted, bool down)
{
    auto r = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();
    const bool isMultiplier = button.getButtonText() == "×10";
    g.setColour (juce::Colours::black.withAlpha (0.85f));
    g.fillRoundedRectangle (r.translated (0.0f, 2.0f), isMultiplier ? 12.0f : 5.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff77736b), r.getX(), r.getY(),
                                             juce::Colour (0xff17191a), r.getRight(), r.getBottom(), false));
    g.fillRoundedRectangle (r, isMultiplier ? 12.0f : 5.0f);
    g.setColour (juce::Colour (0xff0a0b0c));
    g.drawRoundedRectangle (r, isMultiplier ? 12.0f : 5.0f, 1.0f);

    if (isMultiplier)
    {
        const float cx = r.getCentreX();
        const float travel = juce::jmax (5.0f, r.getHeight() * 0.22f);
        const float cy = r.getCentreY() + (on ? travel : -travel);
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffd7d1c3), cx - 5.0f, cy - 10.0f,
                                                 juce::Colour (0xff514d45), cx + 5.0f, cy + 10.0f, false));
        g.fillRoundedRectangle (cx - 7.0f, cy - 13.0f, 14.0f, 26.0f, 6.0f);
        g.setColour (juce::Colour (0xffe0d8c8));
        g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
        g.drawText ("×10", button.getLocalBounds().withY (button.getHeight() - 19), juce::Justification::centred);
    }
    else
    {
        g.setColour (on ? juce::Colour (0xffb5f5a0) : juce::Colour (0xff4b514a));
        g.fillRoundedRectangle (r.getX() + 8.0f, r.getY() + 5.0f, 13.0f, r.getHeight() - 10.0f, 3.0f);
        g.setColour (juce::Colour (0xff0b0c0d));
        g.drawRoundedRectangle (r.getX() + 6.0f, r.getY() + 3.0f, 17.0f, r.getHeight() - 6.0f, 3.0f, 1.0f);
        g.setColour (button.findColour (juce::ToggleButton::textColourId).withAlpha (highlighted || down ? 1.0f : 0.92f));
        g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
        g.drawFittedText (button.getButtonText(), button.getLocalBounds().withTrimmedLeft (26), juce::Justification::centred, 1);
    }
}

void AudioArts4100AudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff080909), 0.0f, 0.0f,
                                             juce::Colour (0xff242321), 0.0f, bounds.getBottom(), false));
    g.fillAll();
    auto panel = bounds.reduced (8.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff292927), 0.0f, 0.0f,
                                             juce::Colour (0xff111313), 0.0f, bounds.getBottom(), false));
    g.fillRoundedRectangle (panel, 7.0f);
    g.setColour (juce::Colour (0xff77736a)); g.drawRoundedRectangle (panel, 7.0f, 1.2f);

    // Subtle brushed-metal grain.
    g.setColour (juce::Colours::white.withAlpha (0.018f));
    for (int y = 12; y < getHeight() - 10; y += 5) g.drawHorizontalLine (y, 12.0f, (float)getWidth() - 12.0f);

    // Four rack screws, one in each corner.
    for (auto pt : { juce::Point<float> (35.0f, 34.0f), juce::Point<float> ((float)getWidth() - 35.0f, 34.0f),
                     juce::Point<float> (35.0f, (float)getHeight() - 34.0f), juce::Point<float> ((float)getWidth() - 35.0f, (float)getHeight() - 34.0f) })
    {
        g.setColour (juce::Colours::black); g.fillEllipse (pt.x - 14.0f, pt.y - 14.0f, 28.0f, 28.0f);
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffb9b4a9), pt.x - 9.0f, pt.y - 10.0f,
                                                 juce::Colour (0xff343638), pt.x + 10.0f, pt.y + 10.0f, false));
        g.fillEllipse (pt.x - 11.0f, pt.y - 11.0f, 22.0f, 22.0f);
        g.setColour (juce::Colour (0xff17191a));
        g.drawLine (pt.x - 5.0f, pt.y - 4.0f, pt.x + 5.0f, pt.y + 4.0f, 2.5f);
        g.drawLine (pt.x - 5.0f, pt.y + 4.0f, pt.x + 5.0f, pt.y - 4.0f, 2.5f);
    }

    g.setColour (juce::Colour (0xff8c877c));
    g.drawHorizontalLine (77, 28.0f, (float)getWidth() - 28.0f);
    g.drawHorizontalLine (624, 28.0f, (float)getWidth() - 28.0f);

    const std::array<juce::Rectangle<float>, 4> areas {
        juce::Rectangle<float> (30, 94, 560, 250), juce::Rectangle<float> (610, 94, 560, 250),
        juce::Rectangle<float> (30, 360, 560, 250), juce::Rectangle<float> (610, 360, 560, 250)
    };
    for (int i = 0; i < 4; ++i)
    {
        auto a = areas[(size_t)i];
        g.setColour (juce::Colour (0xff111313)); g.fillRoundedRectangle (a, 8.0f);
        g.setColour (juce::Colour (0xff898477)); g.drawRoundedRectangle (a, 8.0f, 1.0f);
        g.setColour (getBandColour (i)); g.fillRoundedRectangle (a.getX() + 1.0f, a.getY() + 1.0f, a.getWidth() - 2.0f, 42.0f, 7.0f);
        g.setColour (juce::Colours::black.withAlpha (0.42f));
        g.fillRect (a.getX() + 1.0f, a.getY() + 36.0f, a.getWidth() - 2.0f, 7.0f);
        g.setColour (juce::Colours::white.withAlpha (0.14f));
        g.drawHorizontalLine ((int)a.getY() + 44, a.getX() + 4.0f, a.getRight() - 4.0f);

        // Calibrated scale legends around the three controls.
        g.setColour (juce::Colour (0xffcfc8b9));
        g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
        const float y = a.getY() + 83.0f;
        g.drawText (i < 2 ? "22       50       100       300" : (i == 2 ? "100       200       500       1k" : "180       500       1k       2.1k"),
                    juce::Rectangle<int> ((int)a.getX() + 22, (int)y, 195, 20), juce::Justification::centred);
        g.drawText ("×1     0.6        1        2", juce::Rectangle<int> ((int)a.getX() + 245, (int)y, 150, 20), juce::Justification::centred);
        g.drawText ("-16                 0                 +16", juce::Rectangle<int> ((int)a.getX() + 392, (int)y, 155, 20), juce::Justification::centred);
    }

    // Footer legend and status lamps.
    auto led = [&g] (float x, float y, juce::Colour c)
    {
        g.setColour (c.darker (0.8f)); g.fillEllipse (x - 12.0f, y - 12.0f, 24.0f, 24.0f);
        g.setColour (c); g.fillEllipse (x - 8.0f, y - 8.0f, 16.0f, 16.0f);
        g.setColour (juce::Colours::white.withAlpha (0.45f)); g.fillEllipse (x - 4.0f, y - 5.0f, 6.0f, 5.0f);
    };
    led (122.0f, 710.0f, juce::Colour (0xffed3430));
    led (1080.0f, 710.0f, processor.getOutputPeak() >= 0.98f ? juce::Colour (0xffff3025) : juce::Colour (0xff6c302b));
    g.setColour (juce::Colour (0xffbdb6a8));
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.drawText ("-∞", 500, 757, 35, 18, juce::Justification::centred);
    g.drawText ("+16", 665, 757, 35, 18, juce::Justification::centred);
}

void AudioArts4100AudioProcessorEditor::resized()
{
    titleLabel.setBounds (75, 20, 450, 45);
    subtitleLabel.setBounds (610, 34, 530, 25);

    const std::array<juce::Point<int>, 4> origins {
        juce::Point<int> (30, 94), { 610, 94 }, { 30, 360 }, { 610, 360 }
    };
    for (int i = 0; i < 4; ++i)
    {
        const auto o = origins[(size_t)i];
        // Frequency, bandwidth and gain knobs; the multiplier sits between frequency and bandwidth.
        const std::array<int, 3> knobX { o.x + 34, o.x + 242, o.x + 414 };
        const std::array<int, 3> knobW { 150, 150, 125 };
        for (int j = 0; j < 3; ++j)
        {
            knobs[(size_t)i][(size_t)j].setBounds (knobX[(size_t)j], o.y + 68, knobW[(size_t)j], 145);
            knobLabels[(size_t)i][(size_t)j].setBounds (knobX[(size_t)j] - 4, o.y + 207, knobW[(size_t)j] + 8, 24);
        }
        multiplierButtons[(size_t)i].setBounds (o.x + 190, o.y + 117, 48, 86);
        bandBypassButtons[(size_t)i].setBounds (o.x + 455, o.y + 8, 94, 27);
    }

    masterSlider.setBounds (510, 640, 155, 120);
    masterLabel.setBounds (680, 675, 210, 42);
    eqBypassButton.setBounds (225, 690, 130, 38);
    powerLabel.setBounds (75, 735, 95, 25);
    overloadLabel.setBounds (1015, 735, 130, 25);
}

void AudioArts4100AudioProcessorEditor::timerCallback()
{
    const bool hot = processor.getOutputPeak() >= 0.98f;
    overloadLabel.setColour (juce::Label::textColourId, hot ? juce::Colour (0xffff5148) : juce::Colour (0xffd0c8b8));
    repaint (1000, 680, 100, 70);
}
