set(DUK_UI_TEST_SOURCES
        json.cc
)

add_executable(duk.ui.test ${DUK_UI_TEST_SOURCES})

target_include_directories(duk.ui.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.ui.test
        PRIVATE
        duk::ui
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.ui.test)