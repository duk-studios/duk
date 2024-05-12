generate_shader_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.frag
        NAME "phong"
        INC_DIR ${DUK_SHADER_SRC_DIR}/phong
        SRC_DIR ${DUK_SHADER_SRC_DIR}/phong
)

target_sources(duk_renderer
        PRIVATE
        phong_shader_data_source.h
        phong_shader_data_source.cpp
)