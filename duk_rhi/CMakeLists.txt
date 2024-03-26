project(duk_rhi)

set(DUK_RHI_HEADERS
        include/duk_rhi/rhi.h
        include/duk_rhi/rhi_exception.h
        include/duk_rhi/command/command_queue.h
        include/duk_rhi/command/command_buffer.h
        include/duk_rhi/render_pass.h
        include/duk_rhi/frame_buffer.h
        include/duk_rhi/image.h
        include/duk_rhi/pixel.h
        include/duk_rhi/sampler.h
        include/duk_rhi/vertex_layout.h
        include/duk_rhi/pipeline/graphics_pipeline.h
        include/duk_rhi/pipeline/shader.h
        include/duk_rhi/command/command_scheduler.h
        include/duk_rhi/command/command.h
        include/duk_rhi/pipeline/shader_data_source.h
        include/duk_rhi/pipeline/std_shader_data_source.h
        include/duk_rhi/descriptor.h
        include/duk_rhi/buffer.h
        include/duk_rhi/descriptor_set.h
        include/duk_rhi/image_data_source.h
        include/duk_rhi/rhi_capabilities.h
        include/duk_rhi/pipeline/compute_pipeline.h
        include/duk_rhi/pipeline/pipeline_flags.h
        include/duk_rhi/index_types.h
        )

set(DUK_RHI_SOURCES
        src/duk_rhi/rhi.cpp
        src/duk_rhi/rhi_exception.cpp
        src/duk_rhi/vertex_layout.cpp
        src/duk_rhi/pipeline/graphics_pipeline.cpp
        src/duk_rhi/pipeline/shader.cpp
        src/duk_rhi/command/command_queue.cpp
        src/duk_rhi/render_pass.cpp
        src/duk_rhi/frame_buffer.cpp
        src/duk_rhi/image.cpp
        src/duk_rhi/pixel.cpp
        src/duk_rhi/sampler.cpp
        src/duk_rhi/command/command_scheduler.cpp
        src/duk_rhi/pipeline/shader_data_source.cpp
        src/duk_rhi/pipeline/std_shader_data_source.cpp
        src/duk_rhi/descriptor.cpp
        src/duk_rhi/vulkan/vulkan_image.h
        src/duk_rhi/vulkan/vulkan_image.cpp
        src/duk_rhi/vulkan/vulkan_sampler.h
        src/duk_rhi/vulkan/vulkan_sampler.cpp
        src/duk_rhi/vulkan/command/vulkan_command_queue.h
        src/duk_rhi/vulkan/command/vulkan_command_queue.cpp
        src/duk_rhi/vulkan/vulkan_import.h
        src/duk_rhi/vulkan/vulkan_import.cpp
        src/duk_rhi/vulkan/vulkan_physical_device.h
        src/duk_rhi/vulkan/vulkan_physical_device.cpp
        src/duk_rhi/vulkan/vulkan_query_error.h
        src/duk_rhi/vulkan/vulkan_rhi.h
        src/duk_rhi/vulkan/vulkan_rhi.cpp
        src/duk_rhi/vulkan/vulkan_swapchain.h
        src/duk_rhi/vulkan/vulkan_swapchain.cpp
        src/duk_rhi/vulkan/command/vulkan_command_buffer.h
        src/duk_rhi/vulkan/command/vulkan_command_buffer.cpp
        src/duk_rhi/vulkan/vulkan_frame_buffer.h
        src/duk_rhi/vulkan/vulkan_frame_buffer.cpp
        src/duk_rhi/vulkan/vulkan_render_pass.h
        src/duk_rhi/vulkan/vulkan_render_pass.cpp
        src/duk_rhi/vulkan/command/vulkan_command_scheduler.h
        src/duk_rhi/vulkan/command/vulkan_command_scheduler.cpp
        src/duk_rhi/vulkan/command/vulkan_command.h
        src/duk_rhi/vulkan/command/vulkan_command.cpp
        src/duk_rhi/vulkan/vulkan_handle_pool.h
        src/duk_rhi/vulkan/vulkan_handle_pool.cpp
        src/duk_rhi/vulkan/vulkan_events.h
        src/duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h
        src/duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.cpp
        src/duk_rhi/vulkan/pipeline/vulkan_shader.h
        src/duk_rhi/vulkan/pipeline/vulkan_shader.cpp
        src/duk_rhi/vulkan/vulkan_flags.h
        src/duk_rhi/vulkan/vulkan_vertex.h
        src/duk_rhi/vulkan/vulkan_vertex.cpp
        src/duk_rhi/vulkan/vulkan_resource_manager.h
        src/duk_rhi/vulkan/vulkan_resource_manager.cpp
        src/duk_rhi/buffer.cpp
        src/duk_rhi/vulkan/vulkan_buffer.h
        src/duk_rhi/vulkan/vulkan_buffer.cpp
        src/duk_rhi/descriptor_set.cpp
        src/duk_rhi/vulkan/vulkan_descriptor_set.cpp
        src/duk_rhi/vulkan/vulkan_descriptor_set.h
        src/duk_rhi/vulkan/vulkan_rhi_capabilities.h
        src/duk_rhi/vulkan/vulkan_rhi_capabilities.cpp
        src/duk_rhi/pipeline/compute_pipeline.cpp
        src/duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.h
        src/duk_rhi/vulkan/pipeline/vulkan_compute_pipeline.cpp
        src/duk_rhi/vulkan/pipeline/vulkan_pipeline_flags.h
        src/duk_rhi/vulkan/pipeline/vulkan_pipeline_flags.cpp
        src/duk_rhi/index_types.cpp
        )

add_library(duk_rhi STATIC ${DUK_RHI_SOURCES} ${DUK_RHI_HEADERS})
add_library(duk::duk_rhi ALIAS duk_rhi)

target_include_directories(duk_rhi
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_rhi
        PUBLIC
        # duk
        duk_macros
        duk_hash
        duk_platform
        duk_log
        duk_task
        duk_serial

        # external
        glm::glm

        grapphs::grapphs

        volk::volk_headers
        )

set_target_properties(duk_rhi PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_rhi
        EXPORT duk-targets
)
