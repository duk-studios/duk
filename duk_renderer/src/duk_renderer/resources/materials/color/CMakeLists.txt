generate_painter_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.frag
        NAME "color"
        INC_DIR ${DUK_RENDERER_INC_DIR}/materials/color
        SRC_DIR ${DUK_RENDERER_SRC_DIR}/materials/color
)