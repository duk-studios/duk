generate_shader_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.frag
        NAME "fullscreen"
        INC_DIR ${DUK_SHADER_SRC_DIR}/fullscreen
        SRC_DIR ${DUK_SHADER_SRC_DIR}/fullscreen
)

target_sources(duk_renderer
        PRIVATE
        fullscreen_shader_data_source.h
        fullscreen_shader_data_source.cpp
)