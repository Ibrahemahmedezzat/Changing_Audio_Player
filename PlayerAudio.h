#pragma once
#include <JuceHeader.h>

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
    void goToStart();
    void goToEnd();
    void setGain(float gain);
    void jumpForward(double seconds);
    void jumpBackward(double seconds);

    void setLooping(bool shouldLoop) { isLooping = shouldLoop; }
    bool isLoopingEnabled() const { return isLooping; }
    void setSpeed(double ratio);

    double getCurrentPosition() const;
    double getTotalLength() const;
    void setPosition(double posInSeconds);

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    double currentSampleRate = 0.0;
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    bool isPaused = false;
    bool isLooping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};

