duk_add_shader(duk.renderer.shaders.fullscreen
        NAME "fullscreen"
        GLSL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/fullscreen.vert
        ${CMAKE_CURRENT_LIST_DIR}/fullscreen.frag
        INC_DIR ${DUK_SHADER_INC_DIR}/fullscreen
        SRC_DIR ${DUK_SHADER_SRC_DIR}/fullscreen
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/fullscreen"
        LIBRARIES duk.renderer.shaders.duk
)