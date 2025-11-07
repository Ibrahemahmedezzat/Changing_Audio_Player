cmake_minimum_required(VERSION 3.22)

project(GuiAppExample VERSION 0.0.1)

# --- JUCE ---
set(JUCE_DIR "C:/JUCE")
add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE_build)

# --- vcpkg toolchain ---
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# --- Add the GUI app ---
juce_add_gui_app(GuiAppExample
        PRODUCT_NAME "Gui App Example"
        STANDALONE_APP TRUE)

# --- Source files ---
target_sources(GuiAppExample
        PRIVATE
        Main.cpp
        MainComponent.cpp
        MainComponent.h
        PlayerAudio.h
        PlayerAudio.cpp
        PlayerGUI.h
        PlayerGUI.cpp
)

# --- Libraries ---
target_link_libraries(GuiAppExample
        PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_audio_devices
        juce::juce_audio_basics
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# --- TagLib (vcpkg) ---
find_package(taglib CONFIG REQUIRED)
target_link_libraries(GuiAppExample PRIVATE TagLib::tag TagLib::tag_c)
