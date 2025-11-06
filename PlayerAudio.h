#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void play();
    void pause();
    void stop();

    void restart();
    bool toggleLoop(); 

    void goToStart();
    void goToEnd();
    void setGain(float gain);
    void jumpForward(double seconds);
    void jumpBackward(double seconds);

    void setLooping(bool shouldLoop) { isLooping = shouldLoop; }
    bool isLoopingEnabled() const { return isLooping; }

    // Playlist support
    void loadPlaylist(const std::vector<juce::File>& files);
    void playFileAt(int index);
    int getNumFiles() const { return playlist.size(); }
    juce::String getFileNameAt(int index) const;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    double currentSampleRate = 0.0;

    bool isPaused = false;
    bool isLooping = false;

    std::vector<juce::File> playlist;
    int currentIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
