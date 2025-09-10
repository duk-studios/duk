project(duk_renderer_test)

set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
        )

add_executable(duk.renderer.test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk.renderer.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.renderer.test
        PRIVATE
        duk::renderer
        )
