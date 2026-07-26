duk_add_shader(duk.renderer
        NAME "phong"
        MODULES
        phong.vert
        phong.frag
        HEADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        NAMESPACE "duk::renderer"
        HEADER_INCLUDE_PREFIX "duk_renderer/shader/phong"
        INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)