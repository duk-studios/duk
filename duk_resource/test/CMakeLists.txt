
set(DUK_AUDIO_TEST_SOURCES
        main.cpp
)

add_executable(duk.resource.test ${DUK_AUDIO_TEST_SOURCES})

target_include_directories(duk.resource.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.resource.test
        PRIVATE
        duk::resource
)
