project(duk_events_test)

set(DUK_EVENTS_TEST_SOURCES
        main.cpp
        )

add_executable(duk_events_test ${DUK_EVENTS_TEST_SOURCES})

target_include_directories(duk_events_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_events_test
        PRIVATE
        duk_events
        )

set_target_properties(duk_events_test PROPERTIES CXX_STANDARD 20)
