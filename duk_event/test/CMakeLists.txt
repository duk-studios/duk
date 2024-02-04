project(duk_event_test)

set(DUK_EVENT_TEST_SOURCES
        main.cpp
        )

add_executable(duk_event_test ${DUK_EVENT_TEST_SOURCES})

target_include_directories(duk_event_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_event_test
        PRIVATE
        duk_event
        )

set_target_properties(duk_event_test PROPERTIES CXX_STANDARD 20)
