generate_painter_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/color.frag
        NAME "color"
        INC_DIR ${DUK_MATERIAL_INC_DIR}/color
        SRC_DIR ${DUK_MATERIAL_SRC_DIR}/color
)