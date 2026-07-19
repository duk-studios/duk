duk_add_shader(duk.renderer.shaders.color
        NAME "color"
        GLSL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/color.vert
        ${CMAKE_CURRENT_LIST_DIR}/color.frag
        INC_DIR ${DUK_SHADER_INC_DIR}/color
        SRC_DIR ${DUK_SHADER_SRC_DIR}/color
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/color"
        LIBRARIES duk.renderer.shaders.duk
)