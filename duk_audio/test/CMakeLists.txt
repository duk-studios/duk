project(duk_audio_test)

set(DUK_AUDIO_TEST_SOURCES
        main.cpp
)

add_executable(duk_audio_test ${DUK_AUDIO_TEST_SOURCES})

target_include_directories(duk_audio_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_audio_test
        PRIVATE
        duk_audio
)

set_target_properties(duk_audio_test PROPERTIES CXX_STANDARD 20)
