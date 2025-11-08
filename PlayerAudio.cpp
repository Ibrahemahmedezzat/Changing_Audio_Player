#include "PlayerAudio.h"

// Constructor
PlayerAudio::PlayerAudio() : resampleSource(&transportSource, false, 2)
{
    formatManager.registerBasicFormats();
    transportSource.setGain(1.0f);
}

// Destructor
PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

// Audio setup
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    resampleSource.releaseResources();
    transportSource.releaseResources();
}

// Load file
bool PlayerAudio::loadFile(const juce::File& file)
{
    stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    auto* reader = formatManager.createReaderFor(file);
    if(reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        return true;
    }
    return false;
}

// Playback controls
void PlayerAudio::start() { transportSource.start(); }
void PlayerAudio::stop() { transportSource.stop(); }

void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }
void PlayerAudio::setPosition(double pos) { transportSource.setPosition(pos); }

double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLength() const { return transportSource.getLengthInSeconds(); }
bool PlayerAudio::isPlaying() const { return transportSource.isPlaying(); }

void PlayerAudio::setPlaybackSpeed(float ratio) { resampleSource.setResamplingRatio(ratio); }
