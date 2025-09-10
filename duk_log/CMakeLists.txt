
set(DUK_LOG_HEADERS
        include/duk_log/log.h
        include/duk_log/logger.h
        include/duk_log/sink.h
        include/duk_log/cout_sink.h
        include/duk_log/file_sink.h
        include/duk_log/fmt_sink.h
        )
set(DUK_LOG_SOURCES
        src/duk_log/log.cpp
        src/duk_log/logger.cpp
        src/duk_log/sink.cpp
        src/duk_log/cout_sink.cpp
        src/duk_log/file_sink.cpp
        src/duk_log/fmt_sink.cpp
        )

add_library(duk.log ${DUK_LOG_HEADERS} ${DUK_LOG_SOURCES})
add_library(duk::log ALIAS duk.log)

target_include_directories(duk.log
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${TERMCOLOR_INCLUDE_DIRS}
        )

target_link_libraries(duk.log
        PUBLIC
        duk::macros
        duk::event
        duk::task
        duk::tools
        duk::platform
        )

set_target_properties(duk.log PROPERTIES EXPORT_NAME log)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.log
        EXPORT duk-targets
)
