generate_painter_files(
        GLSL_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/glsl/sprite_color.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/glsl/sprite_color.frag"
        NAME "sprite_color"
        INC_DIR ${DUK_MATERIAL_INC_DIR}/sprites/sprite_color
        SRC_DIR ${DUK_MATERIAL_SRC_DIR}/sprites/sprite_color
)