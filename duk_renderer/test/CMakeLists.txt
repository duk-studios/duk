set(DUK_RENDERER_TEST_SOURCES
        json.cc
        )

add_executable(duk.renderer.test ${DUK_RENDERER_TEST_SOURCES})

target_include_directories(duk.renderer.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.renderer.test
        PRIVATE
        duk::renderer
        Catch2::Catch2WithMain
        )

catch_discover_tests(duk.renderer.test)