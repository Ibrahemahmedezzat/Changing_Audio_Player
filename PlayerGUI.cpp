#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <algorithm>

// ------------------- Constructor -------------------
PlayerGUI::PlayerGUI()
{
    setSize(900, 600);
    formatManager.registerBasicFormats();

    // All buttons
    auto buttons = { &loadButton, &restartButton, &playPauseButton, &stopButton,
                     &nextButton, &prevButton, &muteButton, &loopButton,
                     &goToStartButton, &goToEndButton, &forwardButton, &backwardButton,
                     &addMarkerButton, &setAButton, &setBButton, &abLoopingButton };
    for(auto* btn : buttons)
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Sliders
    volumeSlider.setRange(0.0, 1.0, 0.01); volumeSlider.setValue(0.5); volumeSlider.addListener(this); addAndMakeVisible(volumeSlider);
    speedSlider.setRange(0.5, 2.0, 0.01); speedSlider.setValue(1.0); speedSlider.addListener(this); addAndMakeVisible(speedSlider);
    positionSlider.setRange(0.0, 1.0); positionSlider.addListener(this); addAndMakeVisible(positionSlider);

    // Labels
    auto labels = { &titleLabel, &artistLabel, &albumLabel, &durationLabel };
    for(auto* lbl : labels){ addAndMakeVisible(*lbl); lbl->setColour(juce::Label::textColourId, juce::Colours::white); }
    addAndMakeVisible(currentTimeLabel); currentTimeLabel.setText("00:00", juce::dontSendNotification);

    // Playlist
    playlistBox.addListener(this); addAndMakeVisible(playlistBox);

    // Markers
    markerList.setModel(this); addAndMakeVisible(markerList);

    setLightTheme();
    startTimerHz(30);
    loadSession();
}

// ------------------- Destructor -------------------
PlayerGUI::~PlayerGUI()
{
    saveSession();
}

// ------------------- Audio callbacks -------------------
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected,double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected,sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);

    // A-B Loop
    if(isABLooping && loopEnd>0.0)
    {
        double pos = playerAudio.getCurrentPosition();
        if(pos >= loopEnd) playerAudio.setPosition(loopStart);
    }

    // Normal Loop or Stop at end
    if(playerAudio.getCurrentPosition() >= playerAudio.getLengthInSeconds())
    {
        if(isLooping) playerAudio.setPosition(0.0);
        else playerAudio.stop();
    }
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

// ------------------- Paint -------------------
void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(30,30,30));

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText(titleLabel.getText(), 10, 10, getWidth()-20, 25, juce::Justification::centredLeft);

    g.setFont(16.0f);
    g.drawText(artistLabel.getText() + " | " + albumLabel.getText(), 10, 40, getWidth()-20, 20, juce::Justification::centredLeft);

    if(audioThumbnail.getTotalLength() > 0.0)
    {
        g.setColour(juce::Colours::orange);
        audioThumbnail.drawChannels(g, juce::Rectangle<int>(10, 70, getWidth()-20, 100), 0.0, audioThumbnail.getTotalLength(), 1.0f);
    }
}

// ------------------- Resized -------------------
void PlayerGUI::resized()
{
    int margin=10, btnW=100, btnH=30, y=200;

    loadButton.setBounds(margin,y,btnW,btnH); playPauseButton.setBounds(120,y,btnW,btnH); stopButton.setBounds(230,y,btnW,btnH); restartButton.setBounds(340,y,btnW,btnH);
    prevButton.setBounds(450,y,btnW,btnH); nextButton.setBounds(560,y,btnW,btnH); muteButton.setBounds(670,y,btnW,btnH); loopButton.setBounds(780,y,btnW,btnH);

    y += btnH+margin;
    goToStartButton.setBounds(margin,y,btnW,btnH); goToEndButton.setBounds(120,y,btnW,btnH); forwardButton.setBounds(230,y,btnW,btnH); backwardButton.setBounds(340,y,btnW,btnH);
    addMarkerButton.setBounds(450,y,btnW,btnH); setAButton.setBounds(560,y,btnW,btnH); setBButton.setBounds(670,y,btnW,btnH); abLoopingButton.setBounds(780,y,btnW,btnH);

    y += btnH+margin;
    volumeSlider.setBounds(margin,y,200,20); speedSlider.setBounds(220,y,200,20); positionSlider.setBounds(430,y,350,20); currentTimeLabel.setBounds(790,y,100,20);

    y += 30;
    titleLabel.setBounds(margin,y,400,20); artistLabel.setBounds(420,y,200,20); albumLabel.setBounds(630,y,200,20); durationLabel.setBounds(840,y,60,20);

    y += 30;
    playlistBox.setBounds(margin,y,400,25);
    markerList.setBounds(420,y,480,getHeight()-y-margin);
}

// ------------------- Button callbacks -------------------
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if(button == &setAButton)
        loopStart = playerAudio.getCurrentPosition();
    else if(button == &setBButton)
        loopEnd = playerAudio.getCurrentPosition();
    else if(button == &abLoopingButton)
    {
        if(loopStart >=0 && loopEnd>loopStart)
        {
            isABLooping = !isABLooping;
            abLoopingButton.setButtonText(isABLooping ? "Stop A-B Loop" : "Start A-B Loop");
        }
    }
    else if(button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setButtonText(isLooping ? "Loop On" : "Loop Off");
    }
    else if(button == &muteButton)
    {
        if(!isMuted){ playerAudio.setGain(0.0f); muteButton.setButtonText("Unmute"); }
        else{ playerAudio.setGain((float)volumeSlider.getValue()); muteButton.setButtonText("Mute"); }
        isMuted = !isMuted;
    }
    else if(button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select audio files...", juce::File{}, "*.wav;*.mp3;*.aiff;*.ogg;*.flac");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& chooser)
            {
                auto files = chooser.getResults();
                if(!files.isEmpty())
                {
                    playlist.clear(); playlistBox.clear();
                    for(int i=0;i<files.size();i++)
                    {
                        playlist.push_back(files[i]);
                        playlistBox.addItem(files[i].getFileName(), i+1);
                    }
                    currentTrackIndex = 0; playlistBox.setSelectedId(1, juce::dontSendNotification);
                    if(loadCurrentTrack())
                    {
                        playerAudio.start();
                        isPlaying = true;
                        playPauseButton.setButtonText("Pause");
                    }
                }
            });
    }
    else if(button == &playPauseButton)
    {
        if(!isPlaying)
        {
            if(playerAudio.getCurrentPosition() >= playerAudio.getLengthInSeconds())
                playerAudio.setPosition(0.0);
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

    else if(button == &stopButton)
    {
        playerAudio.stop(); playerAudio.setPosition(0.0); playPauseButton.setButtonText("Play"); isPlaying = false;
    }
    else if(button == &restartButton)
    {
        playerAudio.setPosition(0.0); playerAudio.start(); playPauseButton.setButtonText("Pause"); isPlaying = true;
    }
    else if(button == &nextButton) nextTrack();
    else if(button == &prevButton) prevTrack();
    else if(button == &goToStartButton) playerAudio.setPosition(0.0);
    else if(button == &goToEndButton)
    {
        playerAudio.setPosition(playerAudio.getLengthInSeconds());
        if(isLooping) playerAudio.setPosition(0.0); // Loop On
        else playerAudio.stop(); // Loop Off
    }
    else if(button == &forwardButton) playerAudio.setPosition(std::min(playerAudio.getCurrentPosition()+10.0, playerAudio.getLengthInSeconds()));
    else if(button == &backwardButton) playerAudio.setPosition(std::max(playerAudio.getCurrentPosition()-10.0,0.0));
    else if(button == &addMarkerButton)
    {
        markers.push_back({"Marker "+juce::String(markers.size()+1), playerAudio.getCurrentPosition()});
        markerList.updateContent(); markerList.repaint();
    }
}

// ------------------- Slider callbacks -------------------
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if(slider == &volumeSlider && !isMuted) playerAudio.setGain((float)volumeSlider.getValue());
    else if(slider == &speedSlider) playerAudio.setPlaybackSpeed((float)speedSlider.getValue());
    else if(slider == &positionSlider)
    {
        isDraggingPosition = true;
        double pos = slider->getValue() * playerAudio.getLengthInSeconds();
        playerAudio.setPosition(pos);
        int minutes = (int)pos / 60;
        int seconds = (int)pos % 60;
        currentTimeLabel.setText(juce::String(minutes)+":"+juce::String(seconds).paddedLeft('0',2), juce::dontSendNotification);
        isDraggingPosition = false;
    }
}

// ------------------- ComboBox callbacks -------------------
void PlayerGUI::comboBoxChanged(juce::ComboBox* comboBox)
{
    if(comboBox == &playlistBox)
    {
        int idx = playlistBox.getSelectedId()-1;
        if(idx >=0 && idx < (int)playlist.size()){ currentTrackIndex = idx; loadCurrentTrack(); playerAudio.start(); isPlaying=true; playPauseButton.setButtonText("Pause"); }
    }
}

// ------------------- ChangeListener -------------------
void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster*)
{
    if(!playerAudio.isPlaying() && isPlaying)
    {
        if(isABLooping || isLooping)
            return;

        if(currentTrackIndex + 1 < (int)playlist.size())
        {
            currentTrackIndex++;
            playlistBox.setSelectedId(currentTrackIndex + 1, juce::dontSendNotification);
            if(loadCurrentTrack())
            {
                playerAudio.start();
                isPlaying = true;
                playPauseButton.setButtonText("Pause");
            }
        }
        else
        {
            playPauseButton.setButtonText("Play");
            isPlaying = false;
        }
    }
}

// ------------------- Load Track -------------------
bool PlayerGUI::loadCurrentTrack()
{
    if(currentTrackIndex>=0 && currentTrackIndex<(int)playlist.size())
    {
        auto& file = playlist[currentTrackIndex];
        if(playerAudio.loadFile(file))
        {
            playerAudio.setPosition(0.0);
            audioThumbnail.clear(); audioThumbnail.setSource(new juce::FileInputSource(file));

            TagLib::FileRef f(file.getFullPathName().toStdString().c_str());
            if(!f.isNull() && f.tag())
            {
                auto* tag = f.tag();
                titleLabel.setText(tag->title().isEmpty()?file.getFileName():tag->title().to8Bit(true), juce::dontSendNotification);
                artistLabel.setText(tag->artist().isEmpty()?"Unknown Artist":tag->artist().to8Bit(true), juce::dontSendNotification);
                albumLabel.setText(tag->album().isEmpty()?"Unknown Album":tag->album().to8Bit(true), juce::dontSendNotification);
                durationLabel.setText(juce::String(playerAudio.getLength(),2)+" s", juce::dontSendNotification);
            }
            return true;
        }
    }
    return false;
}

// ------------------- Next / Prev -------------------
void PlayerGUI::nextTrack()
{
    if(!playlist.empty())
    {
        currentTrackIndex = (currentTrackIndex+1)%playlist.size();
        playlistBox.setSelectedId(currentTrackIndex+1);
        loadCurrentTrack();
        playerAudio.start();
        isPlaying = true;
        playPauseButton.setButtonText("Pause");
    }
}

void PlayerGUI::prevTrack()
{
    if(!playlist.empty())
    {
        currentTrackIndex = (currentTrackIndex-1+playlist.size())%playlist.size();
        playlistBox.setSelectedId(currentTrackIndex+1);
        loadCurrentTrack();
        playerAudio.start();
        isPlaying = true;
        playPauseButton.setButtonText("Pause");
    }
}

// ------------------- Session management -------------------
void PlayerGUI::saveSession()
{
    juce::DynamicObject sessionState;
    if(!playlist.empty() && currentTrackIndex>=0)
    {
        sessionState.setProperty("lastFile", playlist[currentTrackIndex].getFullPathName());
        sessionState.setProperty("position", playerAudio.getCurrentPosition());
    }
    juce::var sessionVar(&sessionState);
    juce::String jsonString = juce::JSON::toString(sessionVar,true);
    sessionFile.replaceWithText(jsonString);
}

void PlayerGUI::loadSession()
{
    if(sessionFile.existsAsFile())
    {
        juce::String jsonString = sessionFile.loadFileAsString();
        juce::var sessionVar = juce::JSON::parse(jsonString);
        if(auto* obj = sessionVar.getDynamicObject())
        {
            juce::String lastFile = obj->getProperty("lastFile").toString();
            double position = obj->getProperty("position");

            juce::File lastAudioFile(lastFile);
            if(lastAudioFile.existsAsFile())
            {
                auto it = std::find_if(playlist.begin(), playlist.end(), [&](const juce::File& f){ return f==lastAudioFile; });
                if(it==playlist.end()){ playlist.push_back(lastAudioFile); currentTrackIndex=(int)playlist.size()-1; }
                else currentTrackIndex = (int)std::distance(playlist.begin(),it);
                if(loadCurrentTrack())
                {
                    playerAudio.setPosition(position);
                    playerAudio.start();
                    isPlaying = true;
                    playPauseButton.setButtonText("Pause");
                }
            }
        }
    }
}

// ------------------- Track Markers ListBox -------------------
int PlayerGUI::getNumRows(){ return (int)markers.size(); }

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    g.fillAll(rowIsSelected ? juce::Colour(255,165,0) : juce::Colour(40,40,40));
    if(rowNumber>=0 && rowNumber<(int)markers.size())
    {
        auto& m = markers[rowNumber];
        g.setColour(juce::Colours::white);
        g.drawText(m.name+" ("+juce::String((int)m.position/60).paddedLeft('0',2)+":"+juce::String((int)m.position%60).paddedLeft('0',2)+")",2,0,width-4,height,juce::Justification::centredLeft);
    }
}

void PlayerGUI::listBoxItemClicked(int row,const juce::MouseEvent&)
{
    if(row>=0 && row<(int)markers.size())
    {
        playerAudio.setPosition(markers[row].position); playerAudio.start(); playPauseButton.setButtonText("Pause"); isPlaying=true;
    }
}

// ------------------- Update Position Slider -------------------
void PlayerGUI::updatePositionSlider()
{
    double pos = playerAudio.getCurrentPosition();
    double len = playerAudio.getLengthInSeconds();
    if(len>0.0)
    {
        positionSlider.setValue(pos/len, juce::dontSendNotification);
        int minutes=(int)pos/60; int seconds=(int)pos%60;
        currentTimeLabel.setText(juce::String(minutes)+":"+juce::String(seconds).paddedLeft('0',2), juce::dontSendNotification);
    }
}

// ------------------- Timer callback -------------------
void PlayerGUI::timerCallback()
{
    if(!isDraggingPosition) updatePositionSlider();
    repaint();
}

// ------------------- Helper Functions -------------------
void PlayerGUI::setLightTheme()
{
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    getLookAndFeel().setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::orange);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);
}
