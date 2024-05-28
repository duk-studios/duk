generate_shader_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/text.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/text.frag
        NAME "text"
        INC_DIR ${DUK_SHADER_SRC_DIR}/text
        SRC_DIR ${DUK_SHADER_SRC_DIR}/text
)