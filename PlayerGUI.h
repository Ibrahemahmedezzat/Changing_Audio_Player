#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton forwardButton{ ">> +10s" };
    juce::TextButton backwardButton{ "<< -10s" };
    juce::TextButton loopButton{ "Loop Off" };
    juce::TextButton startButton{ "|<" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" };
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider progressSlider;
    bool userIsDragging = false;

    std::unique_ptr<juce::FileChooser> fileChooser;
    double currentSampleRate = 0.0;
    bool isMuted = false;
    float previousVolume = 1.0f;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;
    void timerCallback() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
