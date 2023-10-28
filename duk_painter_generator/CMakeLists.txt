cmake_minimum_required(VERSION 3.24)
project(duk_painter_generator)

option(DUK_SCENE "Build duk_scene" OFF)
option(DUK_RENDERER "Build duk_renderer" OFF)
option(DUK_SAMPLE "Build duk_sample" OFF)
add_subdirectory(../ ${CMAKE_BINARY_DIR}/duk)

set(DUK_PAINTER_GENERATOR_HEADERS
        include/duk_painter_generator/parser.h
        include/duk_painter_generator/reflector.h
        include/duk_painter_generator/generator.h
        include/duk_painter_generator/types.h
        )

set(DUK_PAINTER_GENERATOR_SOURCES
        src/duk_painter_generator/main.cpp
        src/duk_painter_generator/parser.cpp
        src/duk_painter_generator/reflector.cpp
        src/duk_painter_generator/generator.cpp
        src/duk_painter_generator/types.cpp
        )

add_executable(duk_painter_generator ${DUK_PAINTER_GENERATOR_HEADERS} ${DUK_PAINTER_GENERATOR_SOURCES})

target_include_directories(duk_painter_generator
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_painter_generator
        PRIVATE
        duk_rhi
        cxxopts::cxxopts
        spirv-reflect-static
)

set_target_properties(duk_painter_generator PROPERTIES CXX_STANDARD 20)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

install(TARGETS duk_painter_generator)