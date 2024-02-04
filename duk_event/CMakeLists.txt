project(duk_event)

set(DUK_EVENT_HEADERS
        include/duk_event/event.h
)

set(DUK_EVENT_SOURCES
        src/duk_event/event.cpp
)

add_library(duk_event STATIC ${DUK_EVENT_HEADERS} ${DUK_EVENT_SOURCES})

target_include_directories(duk_event
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_event PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)