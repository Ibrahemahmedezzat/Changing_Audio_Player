#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &playPauseButton, &stopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Labels for metadata
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(albumLabel);
    addAndMakeVisible(durationLabel);

    setSize(500, 250);
}

PlayerGUI::~PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &playPauseButton, &stopButton })
        btn->removeListener(this);

    volumeSlider.removeListener(this);
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

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::skyblue);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 90;
    int buttonHeight = 40;
    int gap = 10;

    loadButton.setBounds(20, y, buttonWidth, buttonHeight);
    restartButton.setBounds(loadButton.getRight() + gap, y, buttonWidth, buttonHeight);
    playPauseButton.setBounds(restartButton.getRight() + gap, y, buttonWidth, buttonHeight);
    stopButton.setBounds(playPauseButton.getRight() + gap, y, buttonWidth, buttonHeight);
    volumeSlider.setBounds(20, 80, getWidth() - 40, 30);

    // Metadata labels
    titleLabel.setBounds(20, 120, getWidth() - 40, 20);
    artistLabel.setBounds(20, 140, getWidth() - 40, 20);
    albumLabel.setBounds(20, 160, getWidth() - 40, 20);
    durationLabel.setBounds(20, 180, getWidth() - 40, 20);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.ogg;*.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                    playPauseButton.setButtonText("Play");
                    isPlaying = false;

                    // --- Use TagLib to get metadata ---
                    TagLib::FileRef f(file.getFullPathName().toStdString().c_str());
                    if (!f.isNull() && f.tag())
                    {
                        auto* tag = f.tag();
                        titleLabel.setText(tag->title().to8Bit(true), juce::dontSendNotification);
                        artistLabel.setText(tag->artist().to8Bit(true), juce::dontSendNotification);
                        albumLabel.setText(tag->album().to8Bit(true), juce::dontSendNotification);
                        durationLabel.setText(juce::String(playerAudio.getLength(), 2) + " s", juce::dontSendNotification);
                    }
                    else
                    {
                        titleLabel.setText(file.getFileName(), juce::dontSendNotification);
                        artistLabel.setText("Unknown", juce::dontSendNotification);
                        albumLabel.setText("Unknown", juce::dontSendNotification);
                        durationLabel.setText(juce::String(playerAudio.getLength(), 2) + " s", juce::dontSendNotification);
                    }
                }
            });
    }
    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
        playPauseButton.setButtonText("Pause");
        isPlaying = true;
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
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}
