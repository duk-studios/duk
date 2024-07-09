project(duk_renderer)

set(DUK_RENDERER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_RENDERER_INC_DIR ${DUK_RENDERER_ROOT}/include/duk_renderer)
set(DUK_RENDERER_SRC_DIR ${DUK_RENDERER_ROOT}/src/duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/renderer.h
        include/duk_renderer/builtins.h
        include/duk_renderer/vertex_types.h
        include/duk_renderer/sort.h
        include/duk_renderer/register_types.h
        include/duk_renderer/texture.h
        include/duk_renderer/components/camera.h
        include/duk_renderer/components/canvas.h
        include/duk_renderer/components/lighting.h
        include/duk_renderer/components/material_slot.h
        include/duk_renderer/components/mesh_slot.h
        include/duk_renderer/components/sprite_renderer.h
        include/duk_renderer/components/text_renderer.h
        include/duk_renderer/components/transform.h
        include/duk_renderer/sprite/sprite.h
        include/duk_renderer/sprite/sprite_cache.h
        include/duk_renderer/sprite/sprite_handler.h
        include/duk_renderer/sprite/sprite_mesh.h
        include/duk_renderer/image/image.h
        include/duk_renderer/image/image_builtins.h
        include/duk_renderer/image/image_handler.h
        include/duk_renderer/image/image_loader_stb.h
        include/duk_renderer/font/font.h
        include/duk_renderer/font/font_atlas.h
        include/duk_renderer/font/font_handler.h
        include/duk_renderer/mesh/draw_buffer.h
        include/duk_renderer/mesh/dynamic_mesh.h
        include/duk_renderer/mesh/empty_mesh.h
        include/duk_renderer/mesh/mesh.h
        include/duk_renderer/mesh/mesh_buffer.h
        include/duk_renderer/mesh/mesh_builtins.h
        include/duk_renderer/mesh/static_mesh.h
        include/duk_renderer/mesh/static_mesh_builder.h
        include/duk_renderer/mesh/static_mesh_data_source.h
        include/duk_renderer/material/globals/global_descriptors.h
        include/duk_renderer/material/material.h
        include/duk_renderer/material/material_handler.h
        include/duk_renderer/material/material_buffer.h
        include/duk_renderer/material/material_data.h
        include/duk_renderer/material/instance_buffer.h
        include/duk_renderer/material/uniform_buffer.h
        include/duk_renderer/material/storage_buffer.h
        include/duk_renderer/material/material_descriptor_set.h
        include/duk_renderer/shader/pipeline_cache.h
        include/duk_renderer/shader/shader_pipeline.h
        include/duk_renderer/shader/shader_module.h
        include/duk_renderer/shader/shader_module_builtins.h
        include/duk_renderer/shader/shader_module_handler.h
        include/duk_renderer/shader/shader_pipeline_data.h
        include/duk_renderer/shader/shader_pipeline_builtins.h
        include/duk_renderer/shader/shader_pipeline_handler.h
        include/duk_renderer/material/globals/lights_types.h
        include/duk_renderer/material/globals/camera_types.h
        include/duk_renderer/material/draw_entry.h
        include/duk_renderer/passes/forward_pass.h
        include/duk_renderer/passes/pass.h
        include/duk_renderer/passes/present_pass.h
        include/duk_renderer/text/text_alignment.h
        include/duk_renderer/text/text_mesh.h
        include/duk_renderer/text/text_mesh_builder.h
        include/duk_renderer/text/text_mesh_cache.h
)

set(DUK_RENDERER_SOURCES
        src/duk_renderer/renderer.cpp
        src/duk_renderer/builtins.cpp
        src/duk_renderer/sort.cpp
        src/duk_renderer/vertex_types.cpp
        src/duk_renderer/register_types.cpp
        src/duk_renderer/texture.cpp
        src/duk_renderer/components/camera.cpp
        src/duk_renderer/components/canvas.cpp
        src/duk_renderer/components/lighting.cpp
        src/duk_renderer/components/material_slot.cpp
        src/duk_renderer/components/mesh_slot.cpp
        src/duk_renderer/components/sprite_renderer.cpp
        src/duk_renderer/components/text_renderer.cpp
        src/duk_renderer/components/transform.cpp
        src/duk_renderer/sprite/sprite.cpp
        src/duk_renderer/sprite/sprite_cache.cpp
        src/duk_renderer/sprite/sprite_handler.cpp
        src/duk_renderer/sprite/sprite_mesh.cpp
        src/duk_renderer/shader/pipeline_cache.cpp
        src/duk_renderer/shader/shader_module.cpp
        src/duk_renderer/shader/shader_module_builtins.cpp
        src/duk_renderer/shader/shader_module_handler.cpp
        src/duk_renderer/shader/shader_pipeline.cpp
        src/duk_renderer/shader/shader_pipeline_builtins.cpp
        src/duk_renderer/shader/shader_pipeline_handler.cpp
        src/duk_renderer/shader/color/color_shader_data_source.h
        src/duk_renderer/shader/color/color_shader_data_source.cpp
        src/duk_renderer/shader/fullscreen/fullscreen_shader_data_source.h
        src/duk_renderer/shader/fullscreen/fullscreen_shader_data_source.cpp
        src/duk_renderer/shader/phong/phong_shader_data_source.h
        src/duk_renderer/shader/phong/phong_shader_data_source.cpp
        src/duk_renderer/shader/text/text_shader_data_source.h
        src/duk_renderer/shader/text/text_shader_data_source.cpp
        src/duk_renderer/mesh/draw_buffer.cpp
        src/duk_renderer/mesh/dynamic_mesh.cpp
        src/duk_renderer/mesh/empty_mesh.cpp
        src/duk_renderer/mesh/mesh.cpp
        src/duk_renderer/mesh/mesh_buffer.cpp
        src/duk_renderer/mesh/mesh_builtins.cpp
        src/duk_renderer/mesh/static_mesh.cpp
        src/duk_renderer/mesh/static_mesh_builder.cpp
        src/duk_renderer/mesh/static_mesh_data_source.cpp
        src/duk_renderer/image/image.cpp
        src/duk_renderer/image/image_builtins.cpp
        src/duk_renderer/image/image_handler.cpp
        src/duk_renderer/image/image_loader_stb.cpp
        src/duk_renderer/font/font.cpp
        src/duk_renderer/font/font_atlas.cpp
        src/duk_renderer/font/font_handler.cpp
        src/duk_renderer/font/freetype/freetype_import.h
        src/duk_renderer/font/freetype/freetype_import.cpp
        src/duk_renderer/font/freetype/freetype_font.h
        src/duk_renderer/font/freetype/freetype_font.cpp
        src/duk_renderer/font/freetype/freetype_font_loader.h
        src/duk_renderer/font/freetype/freetype_font_loader.cpp
        src/duk_renderer/material/material.cpp
        src/duk_renderer/material/material_handler.cpp
        src/duk_renderer/material/material_buffer.cpp
        src/duk_renderer/material/material_data.cpp
        src/duk_renderer/material/instance_buffer.cpp
        src/duk_renderer/material/uniform_buffer.cpp
        src/duk_renderer/material/storage_buffer.cpp
        src/duk_renderer/material/material_descriptor_set.cpp
        src/duk_renderer/material/globals/global_descriptors.cpp
        src/duk_renderer/passes/forward_pass.cpp
        src/duk_renderer/passes/pass.cpp
        src/duk_renderer/passes/present_pass.cpp
        src/duk_renderer/text/text_mesh.cpp
        src/duk_renderer/text/text_mesh_builder.cpp
        src/duk_renderer/text/text_mesh_cache.cpp
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
add_subdirectory(src/duk_renderer/shader)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY src/duk_renderer/shader/glsl
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)

install(
        TARGETS duk_renderer
        EXPORT duk-targets
)

