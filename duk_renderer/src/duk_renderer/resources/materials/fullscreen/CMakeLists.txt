generate_material_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.frag
        NAME "fullscreen"
        INC_DIR ${DUK_MATERIAL_INC_DIR}/fullscreen
        SRC_DIR ${DUK_MATERIAL_SRC_DIR}/fullscreen
)