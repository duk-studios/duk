project(duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/data_source.h
        include/duk_renderer/renderer.h
        include/duk_renderer/renderer_error.h
        include/duk_renderer/command_queue.h
        include/duk_renderer/command_buffer.h
        include/duk_renderer/render_pass.h
        include/duk_renderer/frame_buffer.h
        include/duk_renderer/image.h
        include/duk_renderer/image_sampler.h
        include/duk_renderer/mesh/mesh_data_source.h
        include/duk_renderer/mesh/vertex_data_source.h
        include/duk_renderer/mesh/vertex_layout.h
        include/duk_renderer/mesh/vertex_types.h
        include/duk_renderer/mesh/mesh.h
        include/duk_renderer/pipeline/pipeline.h
        include/duk_renderer/pipeline/pipeline_data_source.h
        include/duk_renderer/pipeline/shader.h
        include/duk_renderer/command_scheduler.h
        include/duk_renderer/command.h
        include/duk_renderer/pipeline/shader_data_source.h
        include/duk_renderer/pipeline/std_shader_data_source.h
        include/duk_renderer/pipeline/descriptor.h
        )

set(DUK_RENDERER_SOURCES
        src/duk_renderer/data_source.cpp
        src/duk_renderer/renderer.cpp
        src/duk_renderer/renderer_error.cpp
        src/duk_renderer/mesh/mesh_data_source.cpp
        src/duk_renderer/mesh/vertex_data_source.cpp
        src/duk_renderer/mesh/vertex_layout.cpp
        src/duk_renderer/pipeline/pipeline.cpp
        src/duk_renderer/pipeline/pipeline_data_source.cpp
        src/duk_renderer/pipeline/shader.cpp
        src/duk_renderer/command_queue.cpp
        src/duk_renderer/render_pass.cpp
        src/duk_renderer/frame_buffer.cpp
        src/duk_renderer/image.cpp
        src/duk_renderer/image_sampler.cpp
        src/duk_renderer/vulkan/vulkan_image.h
        src/duk_renderer/vulkan/vulkan_image.cpp
        src/duk_renderer/vulkan/vulkan_sampler.h
        src/duk_renderer/vulkan/vulkan_sampler.cpp
        src/duk_renderer/vulkan/vulkan_command_queue.h
        src/duk_renderer/vulkan/vulkan_command_queue.cpp
        src/duk_renderer/vulkan/vulkan_debug_messenger.h
        src/duk_renderer/vulkan/vulkan_debug_messenger.cpp
        src/duk_renderer/vulkan/vulkan_import.h
        src/duk_renderer/vulkan/vulkan_physical_device.h
        src/duk_renderer/vulkan/vulkan_physical_device.cpp
        src/duk_renderer/vulkan/vulkan_query_error.h
        src/duk_renderer/vulkan/vulkan_renderer.h
        src/duk_renderer/vulkan/vulkan_renderer.cpp
        src/duk_renderer/vulkan/vulkan_swapchain.h
        src/duk_renderer/vulkan/vulkan_swapchain.cpp
        src/duk_renderer/vulkan/vulkan_command_buffer.h
        src/duk_renderer/vulkan/vulkan_command_buffer.cpp
        src/duk_renderer/vulkan/vulkan_frame_buffer.h
        src/duk_renderer/vulkan/vulkan_frame_buffer.cpp
        src/duk_renderer/vulkan/vulkan_render_pass.h
        src/duk_renderer/vulkan/vulkan_render_pass.cpp
        src/duk_renderer/command_scheduler.cpp
        src/duk_renderer/vulkan/vulkan_command_scheduler.h
        src/duk_renderer/vulkan/vulkan_command_scheduler.cpp
        src/duk_renderer/vulkan/vulkan_command.h
        src/duk_renderer/vulkan/vulkan_command.cpp
        src/duk_renderer/vulkan/vulkan_resource_pool.h
        src/duk_renderer/vulkan/vulkan_resource_pool.cpp
        src/duk_renderer/vulkan/vulkan_events.h
        src/duk_renderer/pipeline/shader_data_source.cpp
        src/duk_renderer/pipeline/std_shader_data_source.cpp
        src/duk_renderer/vulkan/pipeline/vulkan_pipeline.h
        src/duk_renderer/vulkan/pipeline/vulkan_pipeline.cpp
        src/duk_renderer/vulkan/pipeline/vulkan_shader.h
        src/duk_renderer/vulkan/pipeline/vulkan_shader.cpp
        src/duk_renderer/pipeline/descriptor.cpp
        src/duk_renderer/vulkan/pipeline/vulkan_descriptor.h
        src/duk_renderer/vulkan/pipeline/vulkan_descriptor.cpp
        src/duk_renderer/vulkan/vulkan_flags.h
        src/duk_renderer/vulkan/mesh/vulkan_vertex.h
        src/duk_renderer/vulkan/mesh/vulkan_vertex.cpp
        src/duk_renderer/vulkan/vulkan_resource_manager.h
        src/duk_renderer/vulkan/vulkan_resource_manager.cpp
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
        # duk
        duk_macros
        duk_hash
        duk_platform
        duk_log
        duk_task

        # external
        glm::glm

        tl::expected

        grapphs::grapphs

        PRIVATE

        Vulkan::Vulkan
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)
