#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}


// JUCE AudioSource lifecycle
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!readerSource)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}


// File loading
bool PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    auto* reader = formatManager.createReaderFor(file);
    if (reader == nullptr)
        return false;

    // Stop and reset before loading new file
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
    transportSource.setGain(1.0f);
    transportSource.setPosition(0.0);

    return true;
}


// ▶Playback controls
void PlayerAudio::play()   { transportSource.start(); }
void PlayerAudio::pause()  { transportSource.stop();  }
void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::goToStart() { transportSource.setPosition(0.0); }
void PlayerAudio::goToEnd()   { transportSource.setPosition(getLengthInSeconds()); }
void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }

// State queries
bool PlayerAudio::isFinished() const { return transportSource.hasStreamFinished(); }
bool PlayerAudio::isPlaying()  const { return transportSource.isPlaying(); }
double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }
void PlayerAudio::setPosition(double newPosition) { transportSource.setPosition(newPosition); }
