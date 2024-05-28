project(duk_resource_test)

set(DUK_AUDIO_TEST_SOURCES
        main.cpp
)

add_executable(duk_resource_test ${DUK_AUDIO_TEST_SOURCES})

target_include_directories(duk_resource_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_resource_test
        PRIVATE
        duk_resource
)

set_target_properties(duk_resource_test PROPERTIES CXX_STANDARD 20)
