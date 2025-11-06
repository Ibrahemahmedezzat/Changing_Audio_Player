#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        resampleSource.setResamplingRatio(1.0); // السرعة الافتراضية
        return true;
    }
    return false;
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::releaseResources()
{
    resampleSource.releaseResources();
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);

    // Loop handling (لو عندك loop)
    if (isLooping && !transportSource.isPlaying() && transportSource.getLengthInSeconds() > 0)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::play()
{
    if (!transportSource.isPlaying())
        transportSource.start();
}

void PlayerAudio::pause()
{
    if (transportSource.isPlaying())
        transportSource.stop();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
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

void PlayerAudio::setSpeed(double ratio)
{
    if (ratio > 0.1)
        resampleSource.setResamplingRatio(ratio);
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getTotalLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setPosition(double posInSeconds)
{
    transportSource.setPosition(posInSeconds);
}
