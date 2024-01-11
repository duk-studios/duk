project(duk_events)

set(DUK_EVENTS_HEADERS
        include/duk_events/event.h
        )

set(DUK_EVENTS_SOURCES
        src/duk_events/event.cpp
        )

add_library(duk_events STATIC ${DUK_EVENTS_HEADERS} ${DUK_EVENTS_SOURCES})

target_include_directories(duk_events
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_events PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)