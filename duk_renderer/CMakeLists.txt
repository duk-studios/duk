set(DUK_RENDERER_HEADERS
        include/duk_renderer/data_source.h
        include/duk_renderer/macros.h
        include/duk_renderer/renderer.h
        include/duk_renderer/renderer_error.h
        include/duk_renderer/mesh/mesh_data_source.h
        include/duk_renderer/mesh/vertex_data_source.h
        include/duk_renderer/mesh/vertex_layout.h
        include/duk_renderer/mesh/vertex_types.h
        )

set(DUK_RENDERER_SOURCES
        src/duk_renderer/data_source.cpp
        src/duk_renderer/renderer.cpp
        src/duk_renderer/renderer_error.cpp
        src/duk_renderer/mesh/mesh_data_source.cpp
        src/duk_renderer/mesh/vertex_data_source.cpp
        src/duk_renderer/mesh/vertex_layout.cpp
        src/duk_renderer/vulkan/vulkan_renderer.h
        src/duk_renderer/vulkan/vulkan_renderer.cpp
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
        duk_hash
        glm::glm
        tl::expected
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)
