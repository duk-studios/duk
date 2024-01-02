cmake_minimum_required(VERSION 3.24)
project(duk_cli)

set(DUK_CLI_HEADERS
        include/duk_cli/command_line.h
        include/duk_cli/commands/json_parser_generator.h
        )

set(DUK_CLI_SOURCES
        src/duk_cli/main.cpp
        src/duk_cli/command_line.cpp
        src/duk_cli/commands/json_parser_generator.cpp
        )

add_executable(duk_cli ${DUK_CLI_HEADERS} ${DUK_CLI_SOURCES})

target_include_directories(duk_cli
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_cli
        PRIVATE
        duk_log
        cxxopts::cxxopts
        RapidJSON
        )

set_target_properties(duk_cli PROPERTIES CXX_STANDARD 20)
