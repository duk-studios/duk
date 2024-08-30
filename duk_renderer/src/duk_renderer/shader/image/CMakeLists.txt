generate_shader_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/image.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/image.frag
        NAME "image"
        INC_DIR ${DUK_SHADER_SRC_DIR}/image
        SRC_DIR ${DUK_SHADER_SRC_DIR}/image
)