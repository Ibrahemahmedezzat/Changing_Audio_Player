#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

    // Loop handling
    if (isLooping && !transportSource.isPlaying() && transportSource.getLengthInSeconds() > 0)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        transportSource.setPosition(0.0);
        isPaused = false;
        return true;
    }
    return false;
}

void PlayerAudio::play()
{
    if (isPaused)
    {
        transportSource.start();
        isPaused = false;
    }
    else if (!transportSource.isPlaying())
    {
        transportSource.start();
    }
}

void PlayerAudio::pause()
{
    if (transportSource.isPlaying())
    {
        transportSource.stop();
        isPaused = true;
    }
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
    isPaused = false;
}

void PlayerAudio::goToStart() { transportSource.setPosition(0.0); }
void PlayerAudio::goToEnd() { transportSource.setPosition(transportSource.getLengthInSeconds()); }
void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }
void PlayerAudio::jumpForward(double seconds)
{
    transportSource.setPosition(std::min(transportSource.getLengthInSeconds(),
        transportSource.getCurrentPosition() + seconds));
}
void PlayerAudio::jumpBackward(double seconds)
{
    transportSource.setPosition(std::max(0.0, transportSource.getCurrentPosition() - seconds));
}
