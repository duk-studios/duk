project(duk_renderer)

set(DUK_RENDERER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_RENDERER_INC_DIR ${DUK_RENDERER_ROOT}/include/duk_renderer)
set(DUK_RENDERER_SRC_DIR ${DUK_RENDERER_ROOT}/src/duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/renderer.h
        include/duk_renderer/vertex_types.h
        include/duk_renderer/sort.h
        include/duk_renderer/forward/forward_renderer.h
        include/duk_renderer/brushes/mesh.h
        include/duk_renderer/brushes/mesh_data_source.h
        include/duk_renderer/brushes/sprite_brush.h
        include/duk_renderer/brushes/brush.h
        include/duk_renderer/brushes/generic_brush.h
        include/duk_renderer/components/mesh_renderer.h
        include/duk_renderer/components/sprite_renderer.h
        include/duk_renderer/components/transform.h
        include/duk_renderer/components/camera.h
        include/duk_renderer/components/lighting.h
        include/duk_renderer/resources/sprite.h
        include/duk_renderer/resources/sprite_data_source.h
        include/duk_renderer/resources/materials/globals/global_descriptors.h
        include/duk_renderer/resources/materials/painter.h
        include/duk_renderer/resources/materials/material.h
        include/duk_renderer/resources/materials/mesh_material.h
        include/duk_renderer/resources/materials/sprite_material.h
        include/duk_renderer/resources/materials/uniform_buffer.h
        include/duk_renderer/resources/materials/storage_buffer.h
        include/duk_renderer/resources/materials/color/color_material.h
        include/duk_renderer/resources/materials/color/color_shader_data_source.h
        include/duk_renderer/resources/materials/color/color_types.h
        include/duk_renderer/resources/materials/color/color_descriptor_sets.h
        include/duk_renderer/resources/materials/fullscreen/fullscreen_material.h
        include/duk_renderer/resources/materials/fullscreen/fullscreen_shader_data_source.h
        include/duk_renderer/resources/materials/fullscreen/fullscreen_types.h
        include/duk_renderer/resources/materials/fullscreen/fullscreen_descriptor_sets.h
        include/duk_renderer/resources/materials/phong/phong_material.h
        include/duk_renderer/resources/materials/phong/phong_shader_data_source.h
        include/duk_renderer/resources/materials/phong/phong_types.h
        include/duk_renderer/resources/materials/phong/phong_descriptor_sets.h
        include/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_material.h
        include/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_shader_data_source.h
        include/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_types.h
        include/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_descriptor_sets.h
        include/duk_renderer/resources/materials/globals/lights_types.h
        include/duk_renderer/resources/materials/globals/camera_types.h
        include/duk_renderer/resources/materials/draw_entry.h
        include/duk_renderer/passes/forward_pass.h
        include/duk_renderer/passes/pass.h
        include/duk_renderer/passes/present_pass.h
        include/duk_renderer/pools/image_pool.h
        include/duk_renderer/pools/mesh_pool.h
        include/duk_renderer/pools/material_pool.h
        include/duk_renderer/pools/sprite_pool.h
        include/duk_renderer/components/register_components.h
        )

set(DUK_RENDERER_SOURCES
        src/duk_renderer/renderer.cpp
        src/duk_renderer/sort.cpp
        src/duk_renderer/vertex_types.cpp
        src/duk_renderer/forward/forward_renderer.cpp
        src/duk_renderer/brushes/mesh.cpp
        src/duk_renderer/brushes/mesh_data_source.cpp
        src/duk_renderer/brushes/sprite_brush.cpp
        src/duk_renderer/brushes/brush.cpp
        src/duk_renderer/brushes/generic_brush.cpp
        src/duk_renderer/components/mesh_renderer.cpp
        src/duk_renderer/components/sprite_renderer.cpp
        src/duk_renderer/components/transform.cpp
        src/duk_renderer/components/camera.cpp
        src/duk_renderer/components/lighting.cpp
        src/duk_renderer/resources/sprite.cpp
        src/duk_renderer/resources/sprite_data_source.cpp
        src/duk_renderer/resources/materials/painter.cpp
        src/duk_renderer/resources/materials/material.cpp
        src/duk_renderer/resources/materials/mesh_material.cpp
        src/duk_renderer/resources/materials/sprite_material.cpp
        src/duk_renderer/resources/materials/uniform_buffer.cpp
        src/duk_renderer/resources/materials/storage_buffer.cpp
        src/duk_renderer/resources/materials/color/color_material.cpp
        src/duk_renderer/resources/materials/color/color_shader_data_source.cpp
        src/duk_renderer/resources/materials/color/color_descriptor_sets.cpp
        src/duk_renderer/resources/materials/fullscreen/fullscreen_material.cpp
        src/duk_renderer/resources/materials/fullscreen/fullscreen_shader_data_source.cpp
        src/duk_renderer/resources/materials/fullscreen/fullscreen_descriptor_sets.cpp
        src/duk_renderer/resources/materials/phong/phong_material.cpp
        src/duk_renderer/resources/materials/phong/phong_shader_data_source.cpp
        src/duk_renderer/resources/materials/phong/phong_descriptor_sets.cpp
        src/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_material.cpp
        src/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_shader_data_source.cpp
        src/duk_renderer/resources/materials/sprites/sprite_color/sprite_color_descriptor_sets.cpp
        src/duk_renderer/resources/materials/globals/global_descriptors.cpp
        src/duk_renderer/passes/forward_pass.cpp
        src/duk_renderer/passes/pass.cpp
        src/duk_renderer/passes/present_pass.cpp
        src/duk_renderer/pools/image_pool.cpp
        src/duk_renderer/pools/mesh_pool.cpp
        src/duk_renderer/pools/material_pool.cpp
        src/duk_renderer/pools/sprite_pool.cpp
        src/duk_renderer/components/register_components.cpp
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
        duk_resource
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)

# used for generating spv files
add_subdirectory(src/duk_renderer/resources/materials)

add_subdirectory(test)