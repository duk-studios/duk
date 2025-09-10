cmake_minimum_required(VERSION 3.24)
project(duk_shader_generator)

set(DUK_SHADER_GENERATOR_HEADERS
        include/duk_shader_generator/parser.h
        include/duk_shader_generator/reflector.h
        include/duk_shader_generator/generator.h
        include/duk_shader_generator/types.h
        include/duk_shader_generator/file_generators/file_generator.h
        include/duk_shader_generator/file_generators/shader_data_source_file_generator.h
        include/duk_shader_generator/file_generators/generator_utils.h
        )

set(DUK_SHADER_GENERATOR_SOURCES
        src/duk_shader_generator/main.cpp
        src/duk_shader_generator/parser.cpp
        src/duk_shader_generator/reflector.cpp
        src/duk_shader_generator/generator.cpp
        src/duk_shader_generator/types.cpp
        src/duk_shader_generator/file_generators/file_generator.cpp
        src/duk_shader_generator/file_generators/shader_data_source_file_generator.cpp
        src/duk_shader_generator/file_generators/generator_utils.cpp
        )

add_executable(duk.shader_generator ${DUK_SHADER_GENERATOR_HEADERS} ${DUK_SHADER_GENERATOR_SOURCES})
add_executable(duk::shader_generator ALIAS duk.shader_generator)

target_include_directories(duk.shader_generator
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.shader_generator
        PRIVATE
        duk::rhi
        cxxopts::cxxopts
        spirv-reflect::spirv-reflect-static
)
