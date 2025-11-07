#include "MainComponent.h"
#include <algorithm>

// ------------------- Constructor -------------------
MainComponent::MainComponent()
{
    // --- Buttons ---
    for (auto* btn : { &loadButton, &restartButton, &playPauseButton, &stopButton,
                       &nextButton, &prevButton, &muteButton, &loopButton,
                       &goToStartButton, &goToEndButton, &forwardButton, &backwardButton,
                       &addMarkerButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // --- Sliders ---
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    currentTimeLabel.setText("0:00", juce::dontSendNotification);
    addAndMakeVisible(currentTimeLabel);

    // --- Labels ---
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(albumLabel);
    addAndMakeVisible(durationLabel);

    // --- Playlist ---
    playlistBox.addListener(this);
    addAndMakeVisible(playlistBox);

    // --- Track Markers ListBox ---
    markerList.setModel(this);
    addAndMakeVisible(markerList);

    setSize(800, 800);
    setAudioChannels(0, 2); // stereo output

    // --- Load last session ---
    loadSession();
}

// ------------------- Destructor -------------------
MainComponent::~MainComponent()
{
    saveSession();
    shutdownAudio();
}

// ------------------- Audio callbacks -------------------
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);

    // تحديث الـ Position Slider
    if (!isDraggingPosition)
        updatePositionSlider();
}

void MainComponent::releaseResources()
{
    playerAudio.releaseResources();
}

// ------------------- GUI -------------------
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    int x = 20, y = 20, width = 90, height = 40, gap = 10;

    // --- Row 1: main controls ---
    loadButton.setBounds(x, y, width, height);
    restartButton.setBounds(loadButton.getRight() + gap, y, width, height);
    playPauseButton.setBounds(restartButton.getRight() + gap, y, width, height);
    stopButton.setBounds(playPauseButton.getRight() + gap, y, width, height);
    prevButton.setBounds(stopButton.getRight() + gap, y, width, height);
    nextButton.setBounds(prevButton.getRight() + gap, y, width, height);
    goToStartButton.setBounds(nextButton.getRight() + gap, y, width, height);
    goToEndButton.setBounds(goToStartButton.getRight() + gap, y, width, height);

    // --- Forward/Backward 10s ---
    backwardButton.setBounds(goToEndButton.getRight() + gap, y, width, height);
    forwardButton.setBounds(backwardButton.getRight() + gap, y, width, height);

    // --- Row 2: mute & loop ---
    y += height + gap;
    muteButton.setBounds(x, y, width, height);
    loopButton.setBounds(muteButton.getRight() + gap, y, width, height);

    // --- Row 3: volume slider ---
    y += height + gap;
    volumeSlider.setBounds(x, y, getWidth() - 40, 30);

    // --- Row 4: speed slider ---
    y += 40;
    speedSlider.setBounds(x, y, getWidth() - 40, 30);

    // --- Row 5: position slider + current time ---
    y += 40;
    positionSlider.setBounds(x, y, getWidth() - 140, 30);
    currentTimeLabel.setBounds(positionSlider.getRight() + 10, y, 100, 30);

    // --- Playlist ---
    y += 50;
    playlistBox.setBounds(x, y, getWidth() - 40, 30);

    // --- Metadata labels ---
    y += 50;
    titleLabel.setBounds(x, y, getWidth() - 40, 20);
    artistLabel.setBounds(x, y + 25, getWidth() - 40, 20);
    albumLabel.setBounds(x, y + 50, getWidth() - 40, 20);
    durationLabel.setBounds(x, y + 75, getWidth() - 40, 20);

    // --- Track Markers ---
    y += 120;
    addMarkerButton.setBounds(x, y, 120, 30);
    markerList.setBounds(x, y + 40, 300, 200);
}

// ------------------- Button callbacks -------------------
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
    else if (button == &nextButton) nextTrack();
    else if (button == &prevButton) prevTrack();
    else if (button == &goToStartButton) playerAudio.setPosition(0.0);
    else if (button == &goToEndButton)
    {
        if (isLooping)
            playerAudio.setPosition(0.0);
        else
            playerAudio.setPosition(playerAudio.getLength());
    }
    else if (button == &forwardButton)
    {
        auto currentPos = playerAudio.getCurrentPosition();
        auto newPos = std::min(currentPos + 10.0, playerAudio.getLengthInSeconds());
        playerAudio.setPosition(newPos);
    }
    else if (button == &backwardButton)
    {
        auto currentPos = playerAudio.getCurrentPosition();
        auto newPos = std::max(currentPos - 10.0, 0.0);
        playerAudio.setPosition(newPos);
    }
    else if (button == &addMarkerButton)
    {
        double pos = playerAudio.getCurrentPosition();
        juce::String name = "Marker " + juce::String(markers.size() + 1);
        markers.push_back({ name, pos });
        markerList.updateContent();
        markerList.repaint();
    }
}

// ------------------- Slider callbacks -------------------
void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !isMuted)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &speedSlider)
        playerAudio.setPlaybackSpeed((float)speedSlider.getValue());
    else if (slider == &positionSlider)
    {
        isDraggingPosition = true;
        auto pos = slider->getValue() * playerAudio.getLengthInSeconds();
        playerAudio.setPosition(pos);
        int minutes = (int)pos / 60;
        int seconds = (int)pos % 60;
        currentTimeLabel.setText(juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0',2),
                                  juce::dontSendNotification);
        isDraggingPosition = false;
    }
}

// ------------------- ComboBox -------------------
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

// ------------------- ChangeListener -------------------
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

// ------------------- Load Track -------------------
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

// ------------------- Next / Prev -------------------
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

// ------------------- Session management -------------------
void MainComponent::saveSession()
{
    juce::DynamicObject sessionState;

    if (!playlist.empty() && currentTrackIndex >= 0)
    {
        sessionState.setProperty("lastFile", playlist[currentTrackIndex].getFullPathName());
        sessionState.setProperty("position", playerAudio.getCurrentPosition());
    }

    juce::var sessionVar(&sessionState);
    juce::String jsonString = juce::JSON::toString(sessionVar, true);
    sessionFile.replaceWithText(jsonString);
}

void MainComponent::loadSession()
{
    if (sessionFile.existsAsFile())
    {
        juce::String jsonString = sessionFile.loadFileAsString();
        juce::var sessionVar = juce::JSON::parse(jsonString);

        if (auto* obj = sessionVar.getDynamicObject())
        {
            juce::String lastFile = obj->getProperty("lastFile").toString();
            double position = obj->getProperty("position");

            juce::File lastAudioFile(lastFile);
            if (lastAudioFile.existsAsFile())
            {
                auto it = std::find_if(playlist.begin(), playlist.end(),
                    [&](const juce::File& f){ return f == lastAudioFile; });

                if (it == playlist.end())
                {
                    playlist.push_back(lastAudioFile);
                    currentTrackIndex = (int)playlist.size() - 1;
                }
                else
                    currentTrackIndex = (int)std::distance(playlist.begin(), it);

                loadCurrentTrack();
                playerAudio.setPosition(position);
            }
        }
    }
}

// ------------------- Track Markers ListBox -------------------
int MainComponent::getNumRows() { return (int)markers.size(); }

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                     int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);

    if (rowNumber >= 0 && rowNumber < (int)markers.size())
    {
        auto& m = markers[rowNumber];
        g.setColour(juce::Colours::black);
        g.drawText(m.name + " (" + juce::String((int)m.position / 60).paddedLeft('0',2) + ":" +
                   juce::String((int)m.position % 60).paddedLeft('0',2) + ")",
                   2, 0, width-4, height, juce::Justification::centredLeft);
    }
}

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < (int)markers.size())
    {
        playerAudio.setPosition(markers[row].position);
        playerAudio.start();
        playPauseButton.setButtonText("Pause");
        isPlaying = true;
    }
}

// ------------------- Update Position Slider -------------------
void MainComponent::updatePositionSlider()
{
    double pos = playerAudio.getCurrentPosition();
    double len = playerAudio.getLengthInSeconds();
    if (len > 0.0)
    {
        positionSlider.setValue(pos / len, juce::dontSendNotification);
        int minutes = (int)pos / 60;
        int seconds = (int)pos % 60;
        currentTimeLabel.setText(juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0',2),
                                  juce::dontSendNotification);
    }
}
