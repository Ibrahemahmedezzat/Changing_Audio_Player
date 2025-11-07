#pragma once
#include <JuceHeader.h>
#include <memory>

// A simple audio player that wraps juce::AudioTransportSource
// Provides basic playback control, gain, and position management.
class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;


    // JUCE AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;


    // File management
    bool loadFile(const juce::File& audioFile);

    // Playback controls
    void start();   // start playback
    void stop();    // stop playback
    void pause();   // pause playback
    void goToStart();
    void goToEnd();


    // Audio parameters
    void setGain(float gain);


    // Transport info
    bool isPlaying() const            { return transportSource.isPlaying(); }
    bool isFinished() const           { return getPosition() >= getLength() - 0.05; }
    double getPosition() const        { return transportSource.getCurrentPosition(); }
    double getLength() const          { return transportSource.getLengthInSeconds(); }
    void setPosition(double newPos)   { transportSource.setPosition(newPos); }

    juce::AudioTransportSource& getTransportSource() { return transportSource; }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
