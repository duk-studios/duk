generate_shader_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.frag
        NAME "color"
        INC_DIR ${DUK_SHADER_SRC_DIR}/color
        SRC_DIR ${DUK_SHADER_SRC_DIR}/color
)

target_sources(duk_renderer
        PRIVATE
        color_shader_data_source.h
        color_shader_data_source.cpp
)