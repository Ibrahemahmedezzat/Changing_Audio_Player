#include "PlayerAudio.h"
ؤ

PlayerAudio::PlayerAudio() { formatManager.registerBasicFormats(); }
PlayerAudio::~PlayerAudio() { releaseResources(); }

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

    if (isLooping && !transportSource.isPlaying() && transportSource.getLengthInSeconds() > 0)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::releaseResources() { transportSource.releaseResources(); }

bool PlayerAudio::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (!reader) return false;

    readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
    transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
    transportSource.setPosition(0.0);
    isPaused = false;

    // Metadata
    TagLib::FileRef f(file.getFullPathName().toRawUTF8());
    juce::String displayName = file.getFileName();
    if (!f.isNull() && f.tag())
    {
        TagLib::Tag* tag = f.tag();
        if (!tag->title().isEmpty()) displayName = tag->title().toCString(true);
        DBG("Title: " << displayName);
        DBG("Artist: " << tag->artist().toCString(true));
        DBG("Album: " << tag->album().toCString(true));
    }
    double durationSec = reader->lengthInSamples / reader->sampleRate;
    DBG("Duration: " << (int)durationSec / 60 << "m " << (int)durationSec % 60 << "s");
    return true;
}

void PlayerAudio::play() { if (isPaused) transportSource.start(), isPaused = false; else if (!transportSource.isPlaying()) transportSource.start(); }
void PlayerAudio::pause() { if (transportSource.isPlaying()) transportSource.stop(), isPaused = true; }
void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); isPaused = false; }


//jjjj

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    play();
}


bool PlayerAudio::toggleLoop()
{
    isLooping = !isLooping;
    return isLooping;
}


void PlayerAudio::goToStart() { transportSource.setPosition(0.0); }
void PlayerAudio::goToEnd() { transportSource.setPosition(transportSource.getLengthInSeconds()); }
void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }
void PlayerAudio::jumpForward(double seconds) { transportSource.setPosition(std::min(transportSource.getLengthInSeconds(), transportSource.getCurrentPosition() + seconds)); }
void PlayerAudio::jumpBackward(double seconds) { transportSource.setPosition(std::max(0.0, transportSource.getCurrentPosition() - seconds)); }

// Playlist
void PlayerAudio::loadPlaylist(const std::vector<juce::File>& files) { playlist = files; if (!playlist.empty()) playFileAt(0); }
void PlayerAudio::playFileAt(int index)
{
    if (index < 0 || index >= (int)playlist.size()) return;
    currentIndex = index;
    loadFile(playlist[index]);
    play();
}
juce::String PlayerAudio::getFileNameAt(int index) const { return (index >= 0 && index < (int)playlist.size()) ? playlist[index].getFileName() : ""; }

