#pragma once
#include <JuceHeader.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include "PlayerAudio.h"

// --- Track Marker struct ---
struct TrackMarker
{
    juce::String name;    
    double position;      
};

class MainComponent : public juce::AudioAppComponent,
                      public juce::Button::Listener,
                      public juce::Slider::Listener,
                      public juce::ComboBox::Listener,
                      public juce::ChangeListener,
                      public juce::ListBoxModel 
{
public:
    MainComponent();
    ~MainComponent() override;

    // --- Audio callbacks ---
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // --- GUI callbacks ---
    void paint(juce::Graphics& g) override;
    void resized() override;

    // --- Button/Slider/Combo callbacks ---
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // --- ListBoxModel functions for Track Markers ---
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                          int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

private:
    // --- Player ---
    PlayerAudio playerAudio;

    // --- Buttons ---
    juce::TextButton loadButton{"Load"};
    juce::TextButton restartButton{"Restart"};
    juce::TextButton playPauseButton{"Play"};
    juce::TextButton stopButton{"Stop"};
    juce::TextButton nextButton{"Next"};
    juce::TextButton prevButton{"Prev"};
    juce::TextButton muteButton{"Mute"};
    juce::TextButton loopButton{"Loop Off"};
    juce::TextButton goToStartButton{"|<"};
    juce::TextButton goToEndButton{">|"};
    juce::TextButton forwardButton{" >> +10s "};
    juce::TextButton backwardButton{"<< -10s"};
    juce::TextButton addMarkerButton{"Add Marker"};

    // --- Sliders ---
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;        
    juce::Label currentTimeLabel;       

    // --- Labels ---
    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label albumLabel;
    juce::Label durationLabel;

    // --- Playlist ---
    juce::ComboBox playlistBox;
    std::vector<juce::File> playlist;
    int currentTrackIndex = 0;

    // --- Track Markers ---
    std::vector<TrackMarker> markers;
    juce::ListBox markerList;

    // --- File chooser & session ---
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            .getChildFile("audio_session.json");

    // --- Flags ---
    bool isPlaying = false;
    bool isMuted = false;
    bool isLooping = false;
    bool isDraggingPosition = false; 

    // --- Helper functions ---
    void loadCurrentTrack();
    void nextTrack();
    void prevTrack();
    void saveSession();
    void loadSession();
    void updatePositionSlider(); 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
