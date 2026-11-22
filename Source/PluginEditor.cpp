#include "PluginEditor.h"
#include <BinaryData.h>

namespace
{
    constexpr float designWidth = 1536.0f;
    constexpr float designHeight = 1024.0f;

    // Centres measured from the supplied 1536 x 1024 reference artwork.
    const std::array<std::array<juce::Point<float>, 3>, 4> knobCentres {{
        std::array<juce::Point<float>, 3> {{ {160.0f, 294.0f}, {436.0f, 294.0f}, {650.0f, 294.0f} }},
        std::array<juce::Point<float>, 3> {{ {900.0f, 294.0f}, {1173.0f, 294.0f}, {1390.0f, 294.0f} }},
        std::array<juce::Point<float>, 3> {{ {160.0f, 644.0f}, {436.0f, 644.0f}, {650.0f, 644.0f} }},
        std::array<juce::Point<float>, 3> {{ {900.0f, 644.0f}, {1173.0f, 644.0f}, {1390.0f, 644.0f} }}
    }};

    const std::array<juce::Point<float>, 4> multiplierCentres {{
        juce::Point<float> (294.0f, 302.0f), juce::Point<float> (1030.0f, 302.0f),
        juce::Point<float> (294.0f, 652.0f), juce::Point<float> (1030.0f, 652.0f)
    }};

    const std::array<juce::Point<float>, 4> bypassCentres {{
        juce::Point<float> (690.0f, 136.0f), juce::Point<float> (1430.0f, 136.0f),
        juce::Point<float> (690.0f, 486.0f), juce::Point<float> (1430.0f, 486.0f)
    }};

    const std::array<juce::Point<float>, 4> ledCentres {{
        juce::Point<float> (618.0f, 136.0f), juce::Point<float> (1358.0f, 136.0f),
        juce::Point<float> (618.0f, 486.0f), juce::Point<float> (1358.0f, 486.0f)
    }};

    const std::array<juce::Colour, 4> ledColours {{
        juce::Colour (0xff44ff55), juce::Colour (0xff44ff55),
        juce::Colour (0xff44ff55), juce::Colour (0xff44ff55)
    }};
}

juce::String AudioArts4100AudioProcessorEditor::parameterId (int band, const juce::String& suffix)
{
    return "b" + juce::String (band + 1) + "_" + suffix;
}

void AudioArts4100AudioProcessorEditor::InvisibleLookAndFeel::drawRotarySlider (
    juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&)
{
    // The supplied artwork contains the complete physical knob. The editor paints
    // only its moving pointer over the artwork, while this slider handles input.
}

void AudioArts4100AudioProcessorEditor::InvisibleLookAndFeel::drawButtonBackground (
    juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool)
{
    // Click targets sit exactly over the switches printed in the reference image.
}

void AudioArts4100AudioProcessorEditor::InvisibleLookAndFeel::drawButtonText (
    juce::Graphics&, juce::TextButton&, bool, bool)
{
}

void AudioArts4100AudioProcessorEditor::InvisibleLookAndFeel::drawToggleButton (
    juce::Graphics&, juce::ToggleButton&, bool, bool)
{
}

AudioArts4100AudioProcessorEditor::AudioArts4100AudioProcessorEditor (AudioArts4100AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    panelBackground = juce::ImageFileFormat::loadFrom (BinaryData::PanelBackground_png,
                                                        BinaryData::PanelBackground_pngSize);

    setSize (1200, 800);
    setResizable (true, true);
    getConstrainer()->setFixedAspectRatio (designWidth / designHeight);
    setResizeLimits (960, 640, 1920, 1280);

    for (int band = 0; band < 4; ++band)
    {
        for (int control = 0; control < 3; ++control)
        {
            auto& slider = knobs[(size_t) band][(size_t) control];
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            slider.setRotaryParameters (juce::MathConstants<float>::degreesToRadians (225.0f),
                                        juce::MathConstants<float>::degreesToRadians (495.0f), true);
            slider.setPopupDisplayEnabled (true, false, this);
            slider.setLookAndFeel (&invisibleLook);
            slider.setDoubleClickReturnValue (true, control == 2 ? 0.0 :
                                              (control == 1 ? 1.0 :
                                               (band < 2 ? 161.0 : (band == 2 ? 550.0 : 1140.0))));
            addAndMakeVisible (slider);
        }

        sliderAttachments[(size_t) band][0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (processor.parameters, parameterId (band, "freq"), knobs[(size_t) band][0]);
        sliderAttachments[(size_t) band][1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (processor.parameters, parameterId (band, "width"), knobs[(size_t) band][1]);
        sliderAttachments[(size_t) band][2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (processor.parameters, parameterId (band, "gain"), knobs[(size_t) band][2]);

        auto& multiplier = multiplierButtons[(size_t) band];
        multiplier.setButtonText ("×10");
        multiplier.setClickingTogglesState (true);
        multiplier.setLookAndFeel (&invisibleLook);
        addAndMakeVisible (multiplier);
        multiplierAttachments[(size_t) band] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (processor.parameters, parameterId (band, "x10"), multiplier);

        auto& bypass = bandBypassButtons[(size_t) band];
        bypass.setButtonText ("BYPASS");
        bypass.setClickingTogglesState (true);
        bypass.setLookAndFeel (&invisibleLook);
        addAndMakeVisible (bypass);
        bypassAttachments[(size_t) band] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (processor.parameters, parameterId (band, "bypass"), bypass);
    }

    masterSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    masterSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    masterSlider.setRotaryParameters (juce::MathConstants<float>::degreesToRadians (225.0f),
                                      juce::MathConstants<float>::degreesToRadians (495.0f), true);
    masterSlider.setSkewFactorFromMidPoint (0.0);
    masterSlider.setPopupDisplayEnabled (true, false, this);
    masterSlider.setLookAndFeel (&invisibleLook);
    addAndMakeVisible (masterSlider);
    masterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (processor.parameters, "master", masterSlider);

    eqBypassButton.setButtonText ("EQ BYPASS");
    eqBypassButton.setClickingTogglesState (true);
    eqBypassButton.setLookAndFeel (&invisibleLook);
    addAndMakeVisible (eqBypassButton);
    eqBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (processor.parameters, "eq_bypass", eqBypassButton);

    startTimerHz (15);
}

AudioArts4100AudioProcessorEditor::~AudioArts4100AudioProcessorEditor()
{
    stopTimer();
    for (auto& row : knobs)
        for (auto& slider : row)
            slider.setLookAndFeel (nullptr);
    masterSlider.setLookAndFeel (nullptr);
    for (auto& button : multiplierButtons) button.setLookAndFeel (nullptr);
    for (auto& button : bandBypassButtons) button.setLookAndFeel (nullptr);
    eqBypassButton.setLookAndFeel (nullptr);
}

void AudioArts4100AudioProcessorEditor::drawNeedle (juce::Graphics& g, const juce::Slider& slider,
                                                      float designX, float designY, float designLength,
                                                      float scaleX, float scaleY) const
{
    const float proportion = (float) slider.valueToProportionOfLength (slider.getValue());
    const float angle = juce::MathConstants<float>::degreesToRadians (-135.0f + proportion * 270.0f);
    const float cx = designX * scaleX;
    const float cy = designY * scaleY;
    const float dx = std::sin (angle) * designLength * scaleX;
    const float dy = -std::cos (angle) * designLength * scaleY;

    g.setColour (juce::Colours::black.withAlpha (0.9f));
    g.drawLine (cx + 1.2f, cy + 1.8f, cx + dx + 1.2f, cy + dy + 1.8f, 7.0f * scaleX);
    g.setColour (juce::Colour (0xfff4eee1));
    g.drawLine (cx, cy, cx + dx, cy + dy, 3.6f * scaleX);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.drawLine (cx - 1.0f, cy - 1.0f, cx + dx - 1.0f, cy + dy - 1.0f, 0.8f * scaleX);
}

void AudioArts4100AudioProcessorEditor::drawLed (juce::Graphics& g, float designX, float designY,
                                                   bool on, juce::Colour onColour,
                                                   float scaleX, float scaleY) const
{
    const float w = 37.0f * scaleX;
    const float h = 35.0f * scaleY;
    auto r = juce::Rectangle<float> (designX * scaleX - w * 0.5f,
                                     designY * scaleY - h * 0.5f, w, h);
    g.setColour (juce::Colour (0xff080a09));
    g.fillRoundedRectangle (r, 5.0f * scaleX);
    g.setColour (juce::Colour (0xff454944));
    g.drawRoundedRectangle (r.reduced (1.0f * scaleX), 4.0f * scaleX, 1.0f * scaleX);
    auto inner = r.reduced (5.0f * scaleX, 5.0f * scaleY);
    if (on)
    {
        g.setColour (onColour.withAlpha (0.25f));
        g.fillRoundedRectangle (inner.expanded (3.0f * scaleX), 4.0f * scaleX);
        g.setGradientFill (juce::ColourGradient (onColour.brighter (0.65f), inner.getX(), inner.getY(),
                                                 onColour.darker (0.45f), inner.getRight(), inner.getBottom(), false));
    }
    else
    {
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a403b), inner.getX(), inner.getY(),
                                                 juce::Colour (0xff101311), inner.getRight(), inner.getBottom(), false));
    }
    g.fillRoundedRectangle (inner, 3.0f * scaleX);
}

void AudioArts4100AudioProcessorEditor::paint (juce::Graphics& g)
{
    const float sx = (float) getWidth() / designWidth;
    const float sy = (float) getHeight() / designHeight;

    if (panelBackground.isValid())
        g.drawImage (panelBackground, getLocalBounds().toFloat());
    else
        g.fillAll (juce::Colour (0xff151515));

    for (int band = 0; band < 4; ++band)
    {
        for (int control = 0; control < 3; ++control)
        {
            const auto centre = knobCentres[(size_t) band][(size_t) control];
            drawNeedle (g, knobs[(size_t) band][(size_t) control], centre.x, centre.y, 58.0f, sx, sy);
        }

        const auto switchCentre = multiplierCentres[(size_t) band];
        const bool x10 = multiplierButtons[(size_t) band].getToggleState();
        const float cx = switchCentre.x * sx;
        const float bodyY = switchCentre.y * sy;
        auto switchBody = juce::Rectangle<float> (cx - 18.0f * sx, bodyY - 37.0f * sy,
                                                   36.0f * sx, 74.0f * sy);
        g.setColour (juce::Colour (0xff080909));
        g.fillRoundedRectangle (switchBody, 13.0f * sx);
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff55534d), switchBody.getX(), switchBody.getY(),
                                                 juce::Colour (0xff111313), switchBody.getRight(), switchBody.getBottom(), false));
        g.fillRoundedRectangle (switchBody.reduced (2.0f * sx), 11.0f * sx);
        g.setColour (juce::Colour (0xff0a0b0b));
        g.drawRoundedRectangle (switchBody.reduced (2.0f * sx), 11.0f * sx, 1.0f * sx);

        const float handleY = bodyY + (x10 ? 17.0f : -17.0f) * sy;
        auto handle = juce::Rectangle<float> (cx - 10.0f * sx, handleY - 17.0f * sy,
                                               20.0f * sx, 34.0f * sy);
        g.setColour (juce::Colours::black.withAlpha (0.75f));
        g.fillRoundedRectangle (handle.translated (1.0f * sx, 2.0f * sy), 8.0f * sx);
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffd5d0c4), handle.getX(), handle.getY(),
                                                 juce::Colour (0xff514e47), handle.getRight(), handle.getBottom(), false));
        g.fillRoundedRectangle (handle, 8.0f * sx);
        g.setColour (juce::Colour (0xfff2eee4).withAlpha (0.65f));
        g.drawLine (cx - 4.0f * sx, handle.getY() + 4.0f * sy,
                    cx - 4.0f * sx, handle.getBottom() - 4.0f * sy, 1.2f * sx);

        const bool active = ! bandBypassButtons[(size_t) band].getToggleState();
        drawLed (g, ledCentres[(size_t) band].x, ledCentres[(size_t) band].y,
                 active, ledColours[(size_t) band], sx, sy);
    }

    drawNeedle (g, masterSlider, 765.0f, 894.0f, 68.0f, sx, sy);

    // The original overload lamp is retained in the background; darken it until
    // the measured output peak reaches the overload threshold.
    const bool overload = processor.getOutputPeak() >= 0.98f;
    if (! overload)
    {
        const float cx = 1378.0f * sx;
        const float cy = 872.0f * sy;
        g.setColour (juce::Colour (0xff26100f));
        g.fillEllipse (cx - 15.0f * sx, cy - 15.0f * sy, 30.0f * sx, 30.0f * sy);
        g.setColour (juce::Colour (0xff59201d));
        g.drawEllipse (cx - 12.0f * sx, cy - 12.0f * sy, 24.0f * sx, 24.0f * sy, 2.0f * sx);
    }

    if (eqBypassButton.getToggleState())
    {
        auto r = juce::Rectangle<float> (286.0f * sx, 842.0f * sy, 70.0f * sx, 58.0f * sy);
        g.setColour (juce::Colours::black.withAlpha (0.28f));
        g.fillRoundedRectangle (r, 5.0f * sx);
        g.setColour (juce::Colours::white.withAlpha (0.22f));
        g.drawRoundedRectangle (r, 5.0f * sx, 1.0f * sx);
    }
}

void AudioArts4100AudioProcessorEditor::resized()
{
    const float sx = (float) getWidth() / designWidth;
    const float sy = (float) getHeight() / designHeight;
    auto place = [sx, sy] (juce::Component& component, float cx, float cy, float w, float h)
    {
        component.setBounds (juce::roundToInt ((cx - w * 0.5f) * sx),
                             juce::roundToInt ((cy - h * 0.5f) * sy),
                             juce::roundToInt (w * sx), juce::roundToInt (h * sy));
    };

    for (int band = 0; band < 4; ++band)
    {
        for (int control = 0; control < 3; ++control)
        {
            const auto centre = knobCentres[(size_t) band][(size_t) control];
            place (knobs[(size_t) band][(size_t) control], centre.x, centre.y, 112.0f, 112.0f);
        }
        const auto mult = multiplierCentres[(size_t) band];
        place (multiplierButtons[(size_t) band], mult.x, mult.y, 52.0f, 86.0f);
        const auto bypass = bypassCentres[(size_t) band];
        place (bandBypassButtons[(size_t) band], bypass.x, bypass.y, 96.0f, 43.0f);
    }

    place (masterSlider, 765.0f, 894.0f, 132.0f, 132.0f);
    place (eqBypassButton, 321.0f, 872.0f, 94.0f, 86.0f);
}

void AudioArts4100AudioProcessorEditor::timerCallback()
{
    repaint();
}
