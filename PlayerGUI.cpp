#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

PlayerGUI::PlayerGUI()
{
    // --- Buttons ---
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &restartButton,
                       &stopButton, &startButton, &endButton, &muteButton,
                       &loopButton, &setAButton, &setBButton, &abLoopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(*btn);
    }

    // --- Sliders ---
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    positionSlider.setRange(0.0, 1.0);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    // --- Time label ---
    addAndMakeVisible(timeLabel);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);

    // --- Metadata labels ---
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(albumLabel);
    addAndMakeVisible(durationLabel);

    // --- Timer for position updates ---
    startTimerHz(20);

    setSize(800, 300);
}

PlayerGUI::~PlayerGUI()
{
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &restartButton,
                       &stopButton, &startButton, &endButton, &muteButton,
                       &loopButton, &setAButton, &setBButton, &abLoopButton })
        btn->removeListener(this);

    volumeSlider.removeListener(this);
    positionSlider.removeListener(this);
}



void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 80;
    int buttonHeight = 35;
    int gap = 10;

    // --- Buttons row ---
    loadButton.setBounds(20, y, buttonWidth, buttonHeight);
    playButton.setBounds(loadButton.getRight() + gap, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(playButton.getRight() + gap, y, buttonWidth, buttonHeight);
    restartButton.setBounds(pauseButton.getRight() + gap, y, buttonWidth, buttonHeight);
    stopButton.setBounds(restartButton.getRight() + gap, y, buttonWidth, buttonHeight);
    startButton.setBounds(stopButton.getRight() + gap, y, buttonWidth, buttonHeight);
    endButton.setBounds(startButton.getRight() + gap, y, buttonWidth, buttonHeight);
    muteButton.setBounds(endButton.getRight() + gap, y, buttonWidth, buttonHeight);
    loopButton.setBounds(muteButton.getRight() + gap, y, buttonWidth, buttonHeight);

    // --- Volume & position sliders ---
    y += buttonHeight + 10;
    volumeSlider.setBounds(20, y, getWidth() - 40, 25);

    y += 35;
    positionSlider.setBounds(20, y, getWidth() - 150, 20);
    timeLabel.setBounds(getWidth() - 120, y - 5, 100, 30);

    // --- A/B Loop buttons ---
    y += 40;
    setAButton.setBounds(20, y, 80, 30);
    setBButton.setBounds(110, y, 80, 30);
    abLoopButton.setBounds(200, y, 120, 30);

    // --- Metadata labels ---
    y += 50;
    titleLabel.setBounds(20, y, getWidth() - 40, 20);
    artistLabel.setBounds(20, y + 20, getWidth() - 40, 20);
    albumLabel.setBounds(20, y + 40, getWidth() - 40, 20);
    durationLabel.setBounds(20, y + 60, getWidth() - 40, 20);
}


void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);

    if (isABLoopActive && isPointASet && isPointBSet)
    {
        double currentPos = playerAudio.getPosition();
        if (currentPos >= pointB)
            playerAudio.setPosition(pointA);
    }
    else if (isLooping && playerAudio.isFinished())
    {
        playerAudio.goToStart();
        playerAudio.start();
    }
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
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

                    // Read metadata using TagLib
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
                    else
                    {
                        titleLabel.setText(file.getFileName(), juce::dontSendNotification);
                        artistLabel.setText("Unknown Artist", juce::dontSendNotification);
                        albumLabel.setText("Unknown Album", juce::dontSendNotification);
                        durationLabel.setText(juce::String(playerAudio.getLength(), 2) + " s",
                                              juce::dontSendNotification);
                    }
                }
            });
    }
    else if (button == &playButton)
        playerAudio.start();
    else if (button == &pauseButton)
        playerAudio.pause();
    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }
    else if (button == &startButton)
        playerAudio.goToStart();
    else if (button == &endButton)
        playerAudio.goToEnd();
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            previousVolume = volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            muteButton.setButtonText("Unmute");
        }
        else
        {
            playerAudio.setGain(previousVolume);
            volumeSlider.setValue(previousVolume, juce::dontSendNotification);
            muteButton.setButtonText("Mute");
        }
        isMuted = !isMuted;
    }
    else if (button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setButtonText(isLooping ? "Loop On" : "Loop Off");
    }
    else if (button == &setAButton)
    {
        pointA = playerAudio.getPosition();
        isPointASet = true;
        setAButton.setButtonText("A: " + juce::String(pointA, 1) + "s");
    }
    else if (button == &setBButton)
    {
        pointB = playerAudio.getPosition();
        isPointBSet = true;
        setBButton.setButtonText("B: " + juce::String(pointB, 1) + "s");
    }
    else if (button == &abLoopButton)
    {
        if (isABLoopActive)
        {
            isABLoopActive = false;
            abLoopButton.setButtonText("A-B Loop Off");
        }
        else if (isPointASet && isPointBSet && pointB > pointA)
        {
            isABLoopActive = true;
            abLoopButton.setButtonText("A-B Loop On");
        }
    }
}



void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !isMuted)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &positionSlider)
    {
        double total = playerAudio.getLength();
        if (total > 0)
            playerAudio.setPosition(positionSlider.getValue() * total);
    }
}


void PlayerGUI::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double total = playerAudio.getLength();

    if (total > 0)
        positionSlider.setValue(currentPos / total, juce::dontSendNotification);

    auto formatTime = [](double seconds)
    {
        int mins = (int)(seconds / 60.0);
        int secs = (int)std::fmod(seconds, 60.0);
        return juce::String::formatted("%02d:%02d", mins, secs);
    };

    timeLabel.setText(formatTime(currentPos) + " / " + formatTime(total), juce::dontSendNotification);
}
