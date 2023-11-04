generate_painter_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.frag
        NAME "phong"
        INC_DIR ${DUK_RENDERER_INC_DIR}/painters/phong
        SRC_DIR ${DUK_RENDERER_SRC_DIR}/painters/phong
)