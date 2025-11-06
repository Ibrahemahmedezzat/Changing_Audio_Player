#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ListBoxModel overrides
    int getNumRows() override { return (int)playlistItems.size(); }
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

private:
    PlayerAudio playerAudio;

    // Buttons
    juce::TextButton loadButton{ "Load" }, playButton{ "Play" }, pauseButton{ "Pause" }, stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" }, forwardButton{ ">> +10s" }, backwardButton{ "<< -10s" }, loopButton{ "Loop Off" };
    juce::TextButton startButton{ "|<" }, endButton{ ">|" }, muteButton{ "Mute" };

    juce::Slider volumeSlider;

    // Playlist
    juce::ListBox playlistBox;
    juce::StringArray playlistItems;
    int currentTrackIndex = -1;

    // Metadata display (title, artist, duration)
    juce::Label metadataLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    double currentSampleRate = 0.0;
    bool isMuted = false;
    float previousVolume = 1.0f;

    void updateMetadataDisplay();
};
