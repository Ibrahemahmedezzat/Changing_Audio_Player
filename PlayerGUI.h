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

    // JUCE component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Audio lifecycle
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

private:

    // Audio playback
    PlayerAudio playerAudio;
    std::unique_ptr<juce::FileChooser> fileChooser;
    double currentSampleRate = 0.0;


    // Playback controls
    juce::TextButton loadButton   { "Load" };
    juce::TextButton playButton   { "Play" };
    juce::TextButton pauseButton  { "Pause" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton   { "Stop" };
    juce::TextButton startButton  { "|<" };
    juce::TextButton endButton    { ">|" };
    juce::TextButton muteButton   { "Mute" };
    juce::TextButton loopButton   { "Loop Off" };

    bool isLooping = false;
    bool isMuted   = false;
    float previousVolume = 1.0f;


    // Volume + position
    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label  timeLabel;


    // A-B looping
    juce::TextButton setAButton   { "Set A" };
    juce::TextButton setBButton   { "Set B" };
    juce::TextButton abLoopButton { "A-B Loop Off" };

    double pointA = 0.0;
    double pointB = 0.0;
    bool isPointASet = false;
    bool isPointBSet = false;
    bool isABLoopActive = false;


    // Metadata display (TagLib)
    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label albumLabel;
    juce::Label durationLabel;

    // Event handling
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
