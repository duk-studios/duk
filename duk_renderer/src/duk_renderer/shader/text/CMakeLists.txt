duk_add_shader(duk.renderer.shaders.text
        NAME "text"
        GLSL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/text.vert
        ${CMAKE_CURRENT_LIST_DIR}/text.frag
        INC_DIR ${DUK_SHADER_INC_DIR}/text
        SRC_DIR ${DUK_SHADER_SRC_DIR}/text
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/text"
        LIBRARIES duk.renderer.shaders.duk
)