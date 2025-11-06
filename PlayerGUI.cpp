#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // Buttons
    addAndMakeVisible(loadButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(startButton);
    addAndMakeVisible(endButton);
    addAndMakeVisible(muteButton);

    loadButton.addListener(this);
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
    restartButton.addListener(this);
    forwardButton.addListener(this);
    backwardButton.addListener(this);
    loopButton.addListener(this);
    startButton.addListener(this);
    endButton.addListener(this);
    muteButton.addListener(this);

    // Volume slider
    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setValue(1.0);
    volumeSlider.addListener(this);

    // Playlist
    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);

    // Metadata label
    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No track loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto topArea = area.removeFromTop(30);
    loadButton.setBounds(topArea.removeFromLeft(60));
    playButton.setBounds(topArea.removeFromLeft(60));
    pauseButton.setBounds(topArea.removeFromLeft(60));
    stopButton.setBounds(topArea.removeFromLeft(60));

    auto buttonArea = area.removeFromTop(30);
    startButton.setBounds(buttonArea.removeFromLeft(50));
    backwardButton.setBounds(buttonArea.removeFromLeft(70));
    restartButton.setBounds(buttonArea.removeFromLeft(70));
    forwardButton.setBounds(buttonArea.removeFromLeft(70));
    endButton.setBounds(buttonArea.removeFromLeft(50));
    loopButton.setBounds(buttonArea.removeFromLeft(70));

    volumeSlider.setBounds(area.removeFromTop(30).reduced(10, 0)); 
    muteButton.setBounds(area.removeFromTop(30).removeFromLeft(60)); 

    playlistBox.setBounds(area.removeFromTop(150));
    metadataLabel.setBounds(area.removeFromTop(30));
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select audio files", juce::File{}, "*.wav;*.mp3;*.flac");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultiple,
            [this](const juce::FileChooser& chooser)
            {
                juce::Array<juce::File> selectedFiles;
                chooser.getResults(selectedFiles);

                if (!selectedFiles.isEmpty())
                {
                    playlistItems.clear();
                    for (auto& f : selectedFiles)
                        playlistItems.add(f.getFileName());

                    std::vector<juce::File> filesVec;
                    for (auto& f : selectedFiles)
                        filesVec.push_back(f);

                    playerAudio.loadPlaylist(filesVec);
                    currentTrackIndex = 0;
                    playlistBox.updateContent();
                    playlistBox.selectRow(currentTrackIndex);
                    updateMetadataDisplay();
                }
            });
    }
    else if (button == &playButton) playerAudio.start();
    else if (button == &pauseButton) playerAudio.pause();
    else if (button == &stopButton) playerAudio.stop();
    else if (button == &restartButton) playerAudio.restart();
    else if (button == &forwardButton) playerAudio.skip(10.0);
    else if (button == &backwardButton) playerAudio.skip(-10.0);
    else if (button == &startButton) playerAudio.jumpToStart(); 
    else if (button == &endButton) playerAudio.jumpToEnd();    
    else if (button == &loopButton)
    {
        bool newLoopState = playerAudio.toggleLoop();
        loopButton.setButtonText(newLoopState ? "Loop On" : "Loop Off");
    }
    else if (button == &muteButton)
    {
        isMuted = !isMuted;
        playerAudio.setGain(isMuted ? 0.0f : previousVolume);
        muteButton.setButtonText(isMuted ? "Unmute" : "Mute");
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        previousVolume = (float)slider->getValue();
        if (!isMuted) playerAudio.setGain(previousVolume);
    }
}

// ListBoxModel overrides
void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= playlistItems.size()) return;

    if (rowIsSelected) g.fillAll(juce::Colours::lightblue);
    g.setColour(juce::Colours::white);
    g.drawText(playlistItems[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistItems.size())
    {
        currentTrackIndex = lastRowSelected;
        playerAudio.playTrack(currentTrackIndex);
        updateMetadataDisplay();
    }
}

void PlayerGUI::updateMetadataDisplay()
{
    if (currentTrackIndex >= 0 && currentTrackIndex < playlistItems.size())
    {
        juce::String displayText = playlistItems[currentTrackIndex];
        metadataLabel.setText(displayText, juce::dontSendNotification);
    }
    else
    {
        metadataLabel.setText("No track selected", juce::dontSendNotification);
    }
}
