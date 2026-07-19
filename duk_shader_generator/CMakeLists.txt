set(DUK_SHADER_GENERATOR_HEADERS
        include/duk_shader_generator/options.h
        include/duk_shader_generator/generator.h
        include/duk_shader_generator/file_generators/generator_utils.h
        include/duk_shader_generator/file_generators/shader_data_source_file_generator.h
        )

set(DUK_SHADER_GENERATOR_CORE_SOURCES
        src/duk_shader_generator/generator.cpp
        src/duk_shader_generator/file_generators/generator_utils.cpp
        src/duk_shader_generator/file_generators/shader_data_source_file_generator.cpp
        )

add_library(duk.shader_generator.core OBJECT ${DUK_SHADER_GENERATOR_HEADERS} ${DUK_SHADER_GENERATOR_CORE_SOURCES})

target_include_directories(duk.shader_generator.core
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.shader_generator.core
        PUBLIC
        duk::rhi
        duk::tools
        )

add_executable(duk.shader_generator src/duk_shader_generator/main.cpp)
add_executable(duk::shader_generator ALIAS duk.shader_generator)

target_link_libraries(duk.shader_generator
        PRIVATE
        duk.shader_generator.core
        cxxopts::cxxopts
        )

if (DUK_BUILD_TESTS)
    add_subdirectory(test)
endif ()
