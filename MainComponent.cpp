#include "MainComponent.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>


MainComponent::MainComponent()
{
    // --- Buttons ---
    for (auto* btn : { &loadButton, &restartButton, &playPauseButton, &stopButton,
                       &nextButton, &prevButton, &muteButton, &loopButton,
                       &goToStartButton, &goToEndButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // --- Volume slider ---
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // --- Metadata labels ---
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(albumLabel);
    addAndMakeVisible(durationLabel);

    // --- Playlist combobox ---
    playlistBox.addListener(this);
    addAndMakeVisible(playlistBox);

    // --- Window setup ---
    setSize(1000, 800);
    setAudioChannels(0, 2); // stereo output
}

//==============================================================================
MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    playerAudio.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("Simple Audio Player", getLocalBounds(), juce::Justification::centredTop, 1);
}

void MainComponent::resized()
{
    int x = 20;
    int y = 50;
    int width = 90;
    int height = 40;
    int gap = 10;

    // --- First row: main controls ---
    loadButton.setBounds(x, y, width, height);
    restartButton.setBounds(loadButton.getRight() + gap, y, width, height);
    playPauseButton.setBounds(restartButton.getRight() + gap, y, width, height);
    stopButton.setBounds(playPauseButton.getRight() + gap, y, width, height);
    prevButton.setBounds(stopButton.getRight() + gap, y, width, height);
    nextButton.setBounds(prevButton.getRight() + gap, y, width, height);
    goToStartButton.setBounds(nextButton.getRight() + gap, y, width, height);
    goToEndButton.setBounds(goToStartButton.getRight() + gap, y, width, height);

    // --- Second row: mute & loop ---
    y += height + gap;
    muteButton.setBounds(x, y, width, height);
    loopButton.setBounds(muteButton.getRight() + gap, y, width, height);

    // --- Third row: volume slider ---
    y += height + gap;
    volumeSlider.setBounds(x, y, getWidth() - 40, 30);

    // --- Fourth row: playlist combo box ---
    y += 50;
    playlistBox.setBounds(x, y, getWidth() - 40, 30);

    // --- Fifth row: metadata labels ---
    y += 50;
    titleLabel.setBounds(x, y, getWidth() - 40, 20);
    artistLabel.setBounds(x, y + 25, getWidth() - 40, 20);
    albumLabel.setBounds(x, y + 50, getWidth() - 40, 20);
    durationLabel.setBounds(x, y + 75, getWidth() - 40, 20);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setButtonText(isLooping ? "Loop On" : "Loop Off");
    }
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            playerAudio.setGain(0.0f);
            muteButton.setButtonText("Unmute");
        }
        else
        {
            playerAudio.setGain((float)volumeSlider.getValue());
            muteButton.setButtonText("Mute");
        }
        isMuted = !isMuted;
    }
    else if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files...", juce::File{},
            "*.wav;*.mp3;*.aiff;*.ogg;*.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& chooser)
            {
                auto files = chooser.getResults();
                if (!files.isEmpty())
                {
                    playlist.clear();
                    playlistBox.clear();
                    for (int i = 0; i < files.size(); ++i)
                    {
                        playlist.push_back(files[i]);
                        playlistBox.addItem(files[i].getFileName(), i + 1);
                    }
                    currentTrackIndex = 0;
                    playlistBox.setSelectedId(1, juce::dontSendNotification);
                    loadCurrentTrack();
                }
            });
    }
    else if (button == &playPauseButton)
    {
        if (!isPlaying)
        {
            playerAudio.start();
            playPauseButton.setButtonText("Pause");
        }
        else
        {
            playerAudio.stop();
            playPauseButton.setButtonText("Play");
        }
        isPlaying = !isPlaying;
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
        playPauseButton.setButtonText("Play");
        isPlaying = false;
    }
    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
        playPauseButton.setButtonText("Pause");
        isPlaying = true;
    }
    else if (button == &nextButton)
        nextTrack();
    else if (button == &prevButton)
        prevTrack();
    else if (button == &goToStartButton)
        playerAudio.setPosition(0.0);
    else if (button == &goToEndButton)
    {
        playerAudio.setPosition(playerAudio.getLength());
        if (isLooping)
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
        }
    }
}


void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !isMuted)
        playerAudio.setGain((float)slider->getValue());
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &playlistBox)
    {
        int idx = playlistBox.getSelectedId() - 1;
        if (idx >= 0 && idx < (int)playlist.size())
        {
            currentTrackIndex = idx;
            loadCurrentTrack();
        }
    }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    if (isLooping && !playerAudio.isPlaying())
    {
        if (playerAudio.getPosition() >= playerAudio.getLength() - 0.05)
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
        }
    }
}

void MainComponent::loadCurrentTrack()
{
    if (currentTrackIndex >= 0 && currentTrackIndex < (int)playlist.size())
    {
        auto& file = playlist[currentTrackIndex];
        if (playerAudio.loadFile(file))
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
            isPlaying = true;
            playPauseButton.setButtonText("Pause");

            TagLib::FileRef f(file.getFullPathName().toStdString().c_str());
            if (!f.isNull() && f.tag())
            {
                auto* tag = f.tag();
                titleLabel.setText(tag->title().isEmpty() ? file.getFileName() : tag->title().to8Bit(true),
                                   juce::dontSendNotification);
                artistLabel.setText(tag->artist().isEmpty() ? "Unknown Artist" : tag->artist().to8Bit(true),
                                    juce::dontSendNotification);
                albumLabel.setText(tag->album().isEmpty() ? "Unknown Album" : tag->album().to8Bit(true),
                                   juce::dontSendNotification);
                durationLabel.setText(juce::String(playerAudio.getLength(), 2) + " s",
                                      juce::dontSendNotification);
            }
        }
    }
}

void MainComponent::nextTrack()
{
    if (!playlist.empty())
    {
        currentTrackIndex = (currentTrackIndex + 1) % playlist.size();
        playlistBox.setSelectedId(currentTrackIndex + 1);
        loadCurrentTrack();
    }
}

void MainComponent::prevTrack()
{
    if (!playlist.empty())
    {
        currentTrackIndex = (currentTrackIndex - 1 + playlist.size()) % playlist.size();
        playlistBox.setSelectedId(currentTrackIndex + 1);
        loadCurrentTrack();
    }
}
