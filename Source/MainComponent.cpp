#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible (statusLabel);
    addAndMakeVisible (meterLabel);

    statusLabel.setJustificationType (juce::Justification::centred);
    meterLabel.setJustificationType (juce::Justification::centred);

    // One input and two outputs. Device/channel routing can be changed
    // from the OS/driver setup while we keep the realtime path minimal.
    setAudioChannels (1, 2);
    startTimerHz (10);
    setSize (700, 420);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlockExpected;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& info)
{
    auto* buffer = info.buffer;
    const int start = info.startSample;
    const int count = info.numSamples;

    if (buffer == nullptr || buffer->getNumChannels() == 0)
        return;

    // The first milestone intentionally performs no DSP.
    // Copy input channel 0 to every output channel without allocating memory.
    const float* input = buffer->getReadPointer (0, start);

    float inPeak = 0.0f;
    for (int i = 0; i < count; ++i)
        inPeak = juce::jmax (inPeak, std::abs (input[i]));

    for (int channel = 1; channel < buffer->getNumChannels(); ++channel)
        buffer->copyFrom (channel, start, buffer->getReadPointer (0, start), count);

    inputPeak.store (inPeak, std::memory_order_relaxed);
    outputPeak.store (inPeak, std::memory_order_relaxed);

    if (inPeak >= 0.999f)
        clipped.store (true, std::memory_order_relaxed);
}

void MainComponent::releaseResources()
{
}

void MainComponent::timerCallback()
{
    const auto sampleRateText = currentSampleRate > 0.0
        ? juce::String (currentSampleRate / 1000.0, 1) + " kHz"
        : "not ready";

    statusLabel.setText (
        "Audio pass-through | " + sampleRateText
        + " | block " + juce::String (currentBlockSize) + " samples",
        juce::dontSendNotification);

    const float peak = inputPeak.load (std::memory_order_relaxed);
    const float db = peak > 0.0f ? juce::Decibels::gainToDecibels (peak) : -100.0f;

    meterLabel.setText (
        "Input peak: " + juce::String (db, 1) + " dBFS"
        + (clipped.exchange (false) ? "   CLIP!" : ""),
        juce::dontSendNotification);

    repaint();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setFont (28.0f);
    g.drawFittedText ("BassToneStudio", getLocalBounds().removeFromTop (100),
                      juce::Justification::centred, 1);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced (40);
    area.removeFromTop (100);
    statusLabel.setBounds (area.removeFromTop (50));
    meterLabel.setBounds (area.removeFromTop (50));
}
