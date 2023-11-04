generate_painter_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.frag
        NAME "color"
        INC_DIR ${DUK_RENDERER_INC_DIR}/painters/color
        SRC_DIR ${DUK_RENDERER_SRC_DIR}/painters/color
)