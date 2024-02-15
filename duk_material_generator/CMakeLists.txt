cmake_minimum_required(VERSION 3.24)
project(duk_material_generator)

set(DUK_MATERIAL_GENERATOR_HEADERS
        include/duk_material_generator/parser.h
        include/duk_material_generator/reflector.h
        include/duk_material_generator/generator.h
        include/duk_material_generator/types.h
        include/duk_material_generator/file_generators/file_generator.h
        include/duk_material_generator/file_generators/types_file_generator.h
        include/duk_material_generator/file_generators/global_types_file_generator.h
        include/duk_material_generator/file_generators/local_types_file_generator.h
        include/duk_material_generator/file_generators/shader_data_source_file_generator.h
        include/duk_material_generator/file_generators/descriptor_set_file_generator.h
        include/duk_material_generator/file_generators/generator_utils.h
        )

set(DUK_MATERIAL_GENERATOR_SOURCES
        src/duk_material_generator/main.cpp
        src/duk_material_generator/parser.cpp
        src/duk_material_generator/reflector.cpp
        src/duk_material_generator/generator.cpp
        src/duk_material_generator/types.cpp
        src/duk_material_generator/file_generators/file_generator.cpp
        src/duk_material_generator/file_generators/types_file_generator.cpp
        src/duk_material_generator/file_generators/global_types_file_generator.cpp
        src/duk_material_generator/file_generators/local_types_file_generator.cpp
        src/duk_material_generator/file_generators/shader_data_source_file_generator.cpp
        src/duk_material_generator/file_generators/descriptor_set_file_generator.cpp
        src/duk_material_generator/file_generators/generator_utils.cpp
        )

add_executable(duk_material_generator ${DUK_MATERIAL_GENERATOR_HEADERS} ${DUK_MATERIAL_GENERATOR_SOURCES})

target_include_directories(duk_material_generator
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_material_generator
        PRIVATE
        duk_rhi
        cxxopts::cxxopts
        spirv-reflect-static
)

set_target_properties(duk_material_generator PROPERTIES CXX_STANDARD 20)
