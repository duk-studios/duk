cmake_minimum_required(VERSION 3.24)
project(duk_cli)

if (NOT MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()


set(DUK_CLI_HEADERS
        include/duk_cli/command_line.h
        include/duk_cli/file_generator.h
        include/duk_cli/commands/json_command.h
        include/duk_cli/commands/pack_command.h
        include/duk_cli/commands/update_command.h
        include/duk_cli/commands/init_command.h
        include/duk_cli/commands/status_command.h
        )

set(DUK_CLI_SOURCES
        src/duk_cli/main.cpp
        src/duk_cli/command_line.cpp
        src/duk_cli/file_generator.cpp
        src/duk_cli/commands/json_command.cpp
        src/duk_cli/commands/pack_command.cpp
        src/duk_cli/commands/update_command.cpp
        src/duk_cli/commands/init_command.cpp
        src/duk_cli/commands/status_command.cpp
        )

add_executable(duk.cli ${DUK_CLI_HEADERS} ${DUK_CLI_SOURCES})
add_executable(duk::cli ALIAS duk.cli)

target_include_directories(duk.cli
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.cli
        PRIVATE
        duk::log
        duk::project
        cxxopts::cxxopts
        )

set_target_properties(duk.cli PROPERTIES
        EXPORT_NAME cli
        OUTPUT_NAME duk)

install(
        TARGETS duk.cli
        RUNTIME DESTINATION bin
)

install(
        TARGETS duk.cli
        EXPORT duk-targets
)
