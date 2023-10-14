project(duk_painter_generator)

set(DUK_PAINTER_GENERATOR_HEADERS
        include/duk_painter_generator/parser.h
        include/duk_painter_generator/reflector.h
        include/duk_painter_generator/generator.h
        )
set(DUK_PAINTER_GENERATOR_SOURCES
        src/duk_painter_generator/main.cpp
        src/duk_painter_generator/parser.cpp
        src/duk_painter_generator/reflector.cpp
        src/duk_painter_generator/generator.cpp
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
        duk_renderer
        cxxopts::cxxopts
        spirv-reflect-static
)