cmake_minimum_required(VERSION 3.22)
project(LiveGate VERSION 0.0.1)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(LiveGate
    COMPANY_NAME "SoyMartes"
    PLUGIN_MANUFACTURER_CODE "SoyM"
    PLUGIN_CODE "LvGt"
    FORMATS VST3
    PRODUCT_NAME "LiveGate"
)

target_sources(LiveGate PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(LiveGate PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(LiveGate PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_plugin_client
)

# Ignora la advertencia de memcpy de HarfBuzz y evita que las advertencias detengan el build
target_compile_options(LiveGate PRIVATE -Wno-error -Wno-nontrivial-memcall)
