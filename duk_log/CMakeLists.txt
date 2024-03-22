project(duk_log)

set(DUK_LOG_HEADERS
        include/duk_log/log.h
        include/duk_log/logger.h
        include/duk_log/sink.h
        include/duk_log/sink_cout.h
        include/duk_log/sink_fmt.h
        )
set(DUK_LOG_SOURCES
        src/duk_log/log.cpp
        src/duk_log/logger.cpp
        src/duk_log/sink.cpp
        src/duk_log/sink_cout.cpp
        src/duk_log/sink_fmt.cpp
        )

add_library(duk_log STATIC ${DUK_LOG_HEADERS} ${DUK_LOG_SOURCES})
add_library(duk::duk_log ALIAS duk_log)

target_include_directories(duk_log
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${TERMCOLOR_INCLUDE_DIRS}
        )

target_link_libraries(duk_log
        PUBLIC
        duk_macros
        duk_event
        duk_task
        duk_tools
        duk_platform
        fmt::fmt-header-only
        )

set_target_properties(duk_log PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_log
        EXPORT duk-targets
)
