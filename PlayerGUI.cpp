#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &restartButton, &stopButton,
                       &forwardButton, &backwardButton, &loopButton, &startButton, &endButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(*btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 80;
    int buttonHeight = 40;
    int spacing = 10;

    int x = 20;
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &stopButton, &restartButton,
                       &backwardButton, &forwardButton, &loopButton, &startButton, &endButton, &muteButton })
    {
        btn->setBounds(x, y, buttonWidth, buttonHeight);
        x += buttonWidth + spacing;
    }

    volumeSlider.setBounds(20, y + buttonHeight + 20, getWidth() - 40, 30);
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
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
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select Audio File...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac"
        );

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    playerAudio.loadFile(file);
            });
    }
    else if (button == &playButton) 
    { playerAudio.play(); }
    else if (button == &pauseButton) 
    { playerAudio.pause(); }
    else if (button == &stopButton)
    { playerAudio.stop(); }
    else if (button == &restartButton) 
    {   playerAudio.stop(); 
       playerAudio.goToStart(); playerAudio.play();
    }
    else if (button == &forwardButton) 
    { playerAudio.jumpForward(10.0); }
    else if (button == &backwardButton) 
    { playerAudio.jumpBackward(10.0); }
    else if (button == &loopButton)
    {
        if (!playerAudio.isLoopingEnabled())
        {
            playerAudio.setLooping(true);
            loopButton.setButtonText("Loop On");
        }
        else
        {
            playerAudio.setLooping(false);
            loopButton.setButtonText("Loop Off");
        }
    }
    else if (button == &startButton) { playerAudio.goToStart(); }
    else if (button == &endButton) { playerAudio.goToEnd(); }
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            previousVolume = volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            isMuted = true;
            muteButton.setButtonText("Unmute");
        }
        else
        {
            playerAudio.setGain(previousVolume);
            volumeSlider.setValue(previousVolume, juce::dontSendNotification);
            isMuted = false;
            muteButton.setButtonText("Mute");
        }
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !isMuted)
        playerAudio.setGain((float)slider->getValue());
}
