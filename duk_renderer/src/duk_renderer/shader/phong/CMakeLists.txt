duk_add_shader(duk.renderer.shaders.phong
        NAME "phong"
        GLSL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/phong.vert
        ${CMAKE_CURRENT_LIST_DIR}/phong.frag
        INC_DIR ${DUK_SHADER_INC_DIR}/phong
        SRC_DIR ${DUK_SHADER_SRC_DIR}/phong
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/phong"
        LIBRARIES duk.renderer.shaders.duk
)