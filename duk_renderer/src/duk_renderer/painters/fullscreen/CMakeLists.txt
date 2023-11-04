generate_painter_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/fullscreen.frag
        NAME "fullscreen"
        INC_DIR ${DUK_RENDERER_INC_DIR}/painters/fullscreen
        SRC_DIR ${DUK_RENDERER_SRC_DIR}/painters/fullscreen
)