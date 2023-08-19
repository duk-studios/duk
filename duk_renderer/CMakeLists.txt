project(duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/forward/forward_renderer.h
        include/duk_renderer/shaders/phong_shader.h
        include/duk_renderer/mesh.h
        include/duk_renderer/mesh_data_source.h
        )

set(DUK_RENDERER_SOURCES
        src/duk_renderer/forward/forward_renderer.cpp
        src/duk_renderer/shaders/phong_shader.cpp
        src/duk_renderer/mesh.cpp
        src/duk_renderer/mesh_data_source.cpp
        )

add_library(duk_renderer STATIC ${DUK_RENDERER_SOURCES} ${DUK_RENDERER_HEADERS})

target_include_directories(duk_renderer
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_renderer
        PUBLIC
        duk_scene
        duk_rhi
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)