project(duk_audio_test)

set(DUK_AUDIO_TEST_SOURCES
        main.cpp
)

add_executable(duk.audio.test ${DUK_AUDIO_TEST_SOURCES})

target_include_directories(duk.audio.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.audio.test
        PRIVATE
        duk::audio
)
