project(duk_renderer)

set(DUK_RENDERER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_RENDERER_INC_DIR ${DUK_RENDERER_ROOT}/include/duk_renderer)
set(DUK_RENDERER_SRC_DIR ${DUK_RENDERER_ROOT}/src/duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/renderer.h
        include/duk_renderer/vertex_types.h
        include/duk_renderer/sort.h
        include/duk_renderer/register_types.h
        include/duk_renderer/brush.h
        include/duk_renderer/generic_brush.h
        include/duk_renderer/texture.h
        include/duk_renderer/mesh_buffer.h
        include/duk_renderer/components/camera.h
        include/duk_renderer/components/canvas.h
        include/duk_renderer/components/lighting.h
        include/duk_renderer/components/mesh_renderer.h
        include/duk_renderer/components/sprite_renderer.h
        include/duk_renderer/components/text_renderer.h
        include/duk_renderer/components/transform.h
        include/duk_renderer/sprite/sprite.h
        include/duk_renderer/sprite/sprite_cache.h
        include/duk_renderer/sprite/sprite_handler.h
        include/duk_renderer/sprite/sprite_mesh.h
        include/duk_renderer/sprite/sprite_pool.h
        include/duk_renderer/image/image.h
        include/duk_renderer/image/image_pool.h
        include/duk_renderer/image/image_handler.h
        include/duk_renderer/image/image_loader_stb.h
        include/duk_renderer/font/font.h
        include/duk_renderer/font/font_atlas.h
        include/duk_renderer/font/font_pool.h
        include/duk_renderer/font/font_handler.h
        include/duk_renderer/mesh/mesh.h
        include/duk_renderer/mesh/mesh_data_source.h
        include/duk_renderer/mesh/mesh_pool.h
        include/duk_renderer/material/globals/global_descriptors.h
        include/duk_renderer/material/pipeline.h
        include/duk_renderer/material/material.h
        include/duk_renderer/material/material_handler.h
        include/duk_renderer/material/instance_buffer.h
        include/duk_renderer/material/uniform_buffer.h
        include/duk_renderer/material/storage_buffer.h
        include/duk_renderer/material/material_descriptor_set.h
        include/duk_renderer/material/color/color_material.h
        include/duk_renderer/material/color/color_shader_data_source.h
        include/duk_renderer/material/color/color_descriptors.h
        include/duk_renderer/material/fullscreen/fullscreen_material.h
        include/duk_renderer/material/fullscreen/fullscreen_shader_data_source.h
        include/duk_renderer/material/fullscreen/fullscreen_descriptors.h
        include/duk_renderer/material/phong/phong_material.h
        include/duk_renderer/material/phong/phong_shader_data_source.h
        include/duk_renderer/material/phong/phong_descriptors.h
        include/duk_renderer/material/sprites/sprite_color/sprite_color_material.h
        include/duk_renderer/material/sprites/sprite_color/sprite_color_shader_data_source.h
        include/duk_renderer/material/sprites/sprite_color/sprite_color_descriptors.h
        include/duk_renderer/material/text/text_material.h
        include/duk_renderer/material/text/text_descriptors.h
        include/duk_renderer/material/text/text_shader_data_source.h
        include/duk_renderer/material/globals/lights_types.h
        include/duk_renderer/material/globals/camera_types.h
        include/duk_renderer/material/draw_entry.h
        include/duk_renderer/material/material_pool.h
        include/duk_renderer/passes/forward_pass.h
        include/duk_renderer/passes/pass.h
        include/duk_renderer/passes/present_pass.h
)

set(DUK_RENDERER_SOURCES
        src/duk_renderer/renderer.cpp
        src/duk_renderer/sort.cpp
        src/duk_renderer/vertex_types.cpp
        src/duk_renderer/register_types.cpp
        src/duk_renderer/brush.cpp
        src/duk_renderer/generic_brush.cpp
        src/duk_renderer/texture.cpp
        src/duk_renderer/mesh_buffer.cpp
        src/duk_renderer/components/camera.cpp
        src/duk_renderer/components/canvas.cpp
        src/duk_renderer/components/lighting.cpp
        src/duk_renderer/components/mesh_renderer.cpp
        src/duk_renderer/components/sprite_renderer.cpp
        src/duk_renderer/components/text_renderer.cpp
        src/duk_renderer/components/transform.cpp
        src/duk_renderer/sprite/sprite.cpp
        src/duk_renderer/sprite/sprite_cache.cpp
        src/duk_renderer/sprite/sprite_handler.cpp
        src/duk_renderer/sprite/sprite_mesh.cpp
        src/duk_renderer/sprite/sprite_pool.cpp
        src/duk_renderer/mesh/mesh.cpp
        src/duk_renderer/mesh/mesh_data_source.cpp
        src/duk_renderer/mesh/mesh_pool.cpp
        src/duk_renderer/image/image.cpp
        src/duk_renderer/image/image_pool.cpp
        src/duk_renderer/image/image_handler.cpp
        src/duk_renderer/image/image_loader_stb.cpp
        src/duk_renderer/font/font.cpp
        src/duk_renderer/font/font_atlas.cpp
        src/duk_renderer/font/font_pool.cpp
        src/duk_renderer/font/font_handler.cpp
        src/duk_renderer/font/freetype/freetype_import.h
        src/duk_renderer/font/freetype/freetype_import.cpp
        src/duk_renderer/font/freetype/freetype_font.h
        src/duk_renderer/font/freetype/freetype_font.cpp
        src/duk_renderer/font/freetype/freetype_font_loader.h
        src/duk_renderer/font/freetype/freetype_font_loader.cpp
        src/duk_renderer/material/pipeline.cpp
        src/duk_renderer/material/material.cpp
        src/duk_renderer/material/material_handler.cpp
        src/duk_renderer/material/instance_buffer.cpp
        src/duk_renderer/material/uniform_buffer.cpp
        src/duk_renderer/material/storage_buffer.cpp
        src/duk_renderer/material/material_descriptor_set.cpp
        src/duk_renderer/material/color/color_material.cpp
        src/duk_renderer/material/color/color_shader_data_source.cpp
        src/duk_renderer/material/color/color_descriptors.cpp
        src/duk_renderer/material/fullscreen/fullscreen_material.cpp
        src/duk_renderer/material/fullscreen/fullscreen_shader_data_source.cpp
        src/duk_renderer/material/fullscreen/fullscreen_descriptors.cpp
        src/duk_renderer/material/phong/phong_material.cpp
        src/duk_renderer/material/phong/phong_shader_data_source.cpp
        src/duk_renderer/material/phong/phong_descriptors.cpp
        src/duk_renderer/material/sprites/sprite_color/sprite_color_material.cpp
        src/duk_renderer/material/sprites/sprite_color/sprite_color_shader_data_source.cpp
        src/duk_renderer/material/sprites/sprite_color/sprite_color_descriptors.cpp
        src/duk_renderer/material/text/text_material.cpp
        src/duk_renderer/material/text/text_descriptors.cpp
        src/duk_renderer/material/text/text_shader_data_source.cpp
        src/duk_renderer/material/globals/global_descriptors.cpp
        src/duk_renderer/material/material_pool.cpp
        src/duk_renderer/passes/forward_pass.cpp
        src/duk_renderer/passes/pass.cpp
        src/duk_renderer/passes/present_pass.cpp
        )

add_library(duk_renderer STATIC ${DUK_RENDERER_SOURCES} ${DUK_RENDERER_HEADERS})
add_library(duk::duk_renderer ALIAS duk_renderer)

target_include_directories(duk_renderer
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_renderer
        PUBLIC
        duk_objects
        duk_rhi
        duk_resource
        Freetype::Freetype
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)

# used for generating spv files
add_subdirectory(src/duk_renderer/material)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_renderer
        EXPORT duk-targets
)
