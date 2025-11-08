#pragma once
#include <JuceHeader.h>
#include <memory>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void start();
    void stop();

    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    bool isPlaying() const;

    void setPlaybackSpeed(float ratio);

    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    double getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource { &transportSource, false, 2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
