project(duk_renderer_test)

set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
        )

add_executable(duk_renderer_test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk_renderer_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_renderer_test
        PRIVATE
        duk_renderer
        )

set_target_properties(duk_renderer_test PROPERTIES CXX_STANDARD 20)
