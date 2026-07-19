duk_add_shader(duk.renderer.shaders.image
        NAME "image"
        GLSL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/image.vert
        ${CMAKE_CURRENT_LIST_DIR}/image.frag
        INC_DIR ${DUK_SHADER_INC_DIR}/image
        SRC_DIR ${DUK_SHADER_SRC_DIR}/image
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/image"
        LIBRARIES duk.renderer.shaders.duk
)