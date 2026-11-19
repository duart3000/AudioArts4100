cmake_minimum_required(VERSION 3.22)
project(AudioArts4100 VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(AudioArts4100
    COMPANY_NAME "AudioArts Inspired"
    BUNDLE_ID "com.audioartsinspired.model4100"
    PLUGIN_MANUFACTURER_CODE AuIn
    PLUGIN_CODE A410
    FORMATS VST3 Standalone
    PRODUCT_NAME "AudioArts 4100"
    DESCRIPTION "Four-band parametric equalizer inspired by the AudioArts 4100"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx EQ)

juce_generate_juce_header(AudioArts4100)
target_sources(AudioArts4100 PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)
target_compile_features(AudioArts4100 PUBLIC cxx_std_17)
target_compile_definitions(AudioArts4100 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(AudioArts4100 PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
