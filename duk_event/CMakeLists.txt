
set(DUK_EVENT_HEADERS
        include/duk_event/event.h
)

set(DUK_EVENT_SOURCES
        src/duk_event/event.cpp
)

add_library(duk.event ${DUK_EVENT_HEADERS} ${DUK_EVENT_SOURCES})
add_library(duk::event ALIAS duk.event)

target_include_directories(duk.event
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk.event PROPERTIES EXPORT_NAME event)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.event
        EXPORT duk-targets
)