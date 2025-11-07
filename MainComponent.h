#pragma once
#include <JuceHeader.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include "PlayerAudio.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Button::Listener,
                      public juce::Slider::Listener,
                      public juce::ComboBox::Listener,
                      public juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    void loadCurrentTrack();
    void nextTrack();
    void prevTrack();

    // --- Player ---
    PlayerAudio playerAudio;

    // --- Buttons ---
    juce::TextButton loadButton{"Load"};
    juce::TextButton restartButton{"Restart"};
    juce::TextButton playPauseButton{"Play"};
    juce::TextButton stopButton{"Stop"};
    juce::TextButton nextButton{"Next"};
    juce::TextButton prevButton{"Prev"};
    juce::TextButton muteButton{"Mute"};
    juce::TextButton loopButton{"Loop Off"};
    juce::TextButton goToStartButton{"|<"};
    juce::TextButton goToEndButton{">|"};

    // --- Slider ---
    juce::Slider volumeSlider;

    // --- Labels for metadata ---
    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label albumLabel;
    juce::Label durationLabel;

    // --- Playlist ---
    juce::ComboBox playlistBox;
    std::vector<juce::File> playlist;
    int currentTrackIndex = 0;

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isPlaying = false;
    bool isMuted = false;
    bool isLooping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
