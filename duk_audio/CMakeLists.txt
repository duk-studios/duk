
set(DUK_AUDIO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_AUDIO_INC_DIR ${DUK_AUDIO_ROOT}/include/duk_audio)
set(DUK_AUDIO_SRC_DIR ${DUK_AUDIO_ROOT}/src/duk_audio)

set(DUK_AUDIO_HEADERS
        include/duk_audio/audio_device.h
        include/duk_audio/audio_engine.h
        include/duk_audio/audio_buffer.h
        include/duk_audio/audio_graph.h
        include/duk_audio/decode.h
        include/duk_audio/audio_player.h
        include/duk_audio/audio_processor.h
        include/duk_audio/register_types.h
        include/duk_audio/clip/audio_clip.h
        include/duk_audio/clip/audio_clip_handler.h
        include/duk_audio/nodes/audio_source_node.h
)

set(DUK_AUDIO_SOURCES
        src/duk_audio/audio_device.cpp
        src/duk_audio/audio_engine.cpp
        src/duk_audio/audio_graph.cpp
        src/duk_audio/decode.cpp
        src/duk_audio/audio_player.cpp
        src/duk_audio/register_types.cpp
        src/duk_audio/clip/audio_clip.cpp
        src/duk_audio/clip/audio_clip_handler.cpp
        src/duk_audio/nodes/audio_source_node.cpp
        src/duk_audio/miniaudio/miniaudio_device.h
        src/duk_audio/miniaudio/miniaudio_device.cpp
        src/duk_audio/miniaudio/miniaudio_import.h
        src/duk_audio/miniaudio/miniaudio_import.cpp
)

add_library(duk.audio ${DUK_AUDIO_SOURCES} ${DUK_AUDIO_HEADERS})
add_library(duk::audio ALIAS duk.audio)

target_include_directories(duk.audio
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${MINIAUDIO_INCLUDE_DIRS}
)

target_link_libraries(duk.audio
        PUBLIC
        duk::log
        duk::platform
        duk::resource
)

set_target_properties(duk.audio PROPERTIES EXPORT_NAME audio)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.audio
        EXPORT duk-targets
)
