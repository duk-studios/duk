
set(DUK_RHI_HEADERS
        include/duk_rhi/instance.h
        include/duk_rhi/exception.h
        include/duk_rhi/frame_buffer.h
        include/duk_rhi/image.h
        include/duk_rhi/pixel.h
        include/duk_rhi/sampler.h
        include/duk_rhi/vertex_layout.h
        include/duk_rhi/binding_layout.h
        include/duk_rhi/buffer.h
        include/duk_rhi/buffer_allocator.h
        include/duk_rhi/image_data_source.h
        include/duk_rhi/capabilities.h
        include/duk_rhi/index_types.h
        include/duk_rhi/pipeline_state.h
        include/duk_rhi/shader.h
        include/duk_rhi/shader_module.h
        include/duk_rhi/shader_data_source.h
        include/duk_rhi/runtime_shader_data_source.h
        include/duk_rhi/pipeline_flags.h
        include/duk_rhi/command_queue.h
        include/duk_rhi/command_context.h
        include/duk_rhi/pipeline_state_stack.h
        include/duk_rhi/shader_compiler.h
        include/duk_rhi/shader_cache.h
        include/duk_rhi/shader_reflection.h
        )

set(DUK_RHI_SOURCES
        src/duk_rhi/instance.cpp
        src/duk_rhi/exception.cpp
        src/duk_rhi/vertex_layout.cpp
        src/duk_rhi/binding_layout.cpp
        src/duk_rhi/frame_buffer.cpp
        src/duk_rhi/image.cpp
        src/duk_rhi/pixel.cpp
        src/duk_rhi/sampler.cpp
        src/duk_rhi/index_types.cpp
        src/duk_rhi/shader.cpp
        src/duk_rhi/shader_data_source.cpp
        src/duk_rhi/runtime_shader_data_source.cpp
        src/duk_rhi/shader_reflection.cpp
        src/duk_rhi/command_queue.cpp
        src/duk_rhi/command_context.cpp
        src/duk_rhi/buffer_allocator.cpp
        src/duk_rhi/pipeline_state_stack.cpp
        src/duk_rhi/shader_compiler.cpp
        src/duk_rhi/shader_cache.cpp
        src/duk_rhi/vulkan/vulkan_image.h
        src/duk_rhi/vulkan/vulkan_image.cpp
        src/duk_rhi/vulkan/vulkan_sampler.h
        src/duk_rhi/vulkan/vulkan_sampler.cpp
        src/duk_rhi/vulkan/vulkan_import.h
        src/duk_rhi/vulkan/vulkan_import.cpp
        src/duk_rhi/vulkan/vulkan_physical_device.h
        src/duk_rhi/vulkan/vulkan_physical_device.cpp
        src/duk_rhi/vulkan/vulkan_queue.h
        src/duk_rhi/vulkan/vulkan_queue.cpp
        src/duk_rhi/vulkan/vulkan_instance.h
        src/duk_rhi/vulkan/vulkan_instance.cpp
        src/duk_rhi/vulkan/vulkan_swapchain.h
        src/duk_rhi/vulkan/vulkan_swapchain.cpp
        src/duk_rhi/vulkan/vulkan_frame_buffer.h
        src/duk_rhi/vulkan/vulkan_frame_buffer.cpp
        src/duk_rhi/vulkan/vulkan_handle_pool.h
        src/duk_rhi/vulkan/vulkan_handle_pool.cpp
        src/duk_rhi/vulkan/vulkan_events.h
        src/duk_rhi/vulkan/vulkan_flags.h
        src/duk_rhi/vulkan/vulkan_vertex.h
        src/duk_rhi/vulkan/vulkan_vertex.cpp
        src/duk_rhi/vulkan/vulkan_command_context.h
        src/duk_rhi/vulkan/vulkan_command_context.cpp
        src/duk_rhi/vulkan/vulkan_deletion_queue.h
        src/duk_rhi/vulkan/vulkan_deletion_queue.cpp
        src/duk_rhi/vulkan/vulkan_buffer.h
        src/duk_rhi/vulkan/vulkan_buffer.cpp
        src/duk_rhi/vulkan/vulkan_descriptor_set.cpp
        src/duk_rhi/vulkan/vulkan_descriptor_set.h
        src/duk_rhi/vulkan/vulkan_descriptor_set_allocator.h
        src/duk_rhi/vulkan/vulkan_descriptor_set_allocator.cpp
        src/duk_rhi/vulkan/vulkan_render_state.h
        src/duk_rhi/vulkan/vulkan_render_state.cpp
        src/duk_rhi/vulkan/vulkan_static_render_state.h
        src/duk_rhi/vulkan/vulkan_static_render_state.cpp
        src/duk_rhi/vulkan/vulkan_resource_state.h
        src/duk_rhi/vulkan/vulkan_single_set_resource_state.h
        src/duk_rhi/vulkan/vulkan_single_set_resource_state.cpp
        src/duk_rhi/vulkan/vulkan_shader.h
        src/duk_rhi/vulkan/vulkan_shader.cpp
)

add_library(duk.rhi ${DUK_RHI_SOURCES} ${DUK_RHI_HEADERS})
add_library(duk::rhi ALIAS duk.rhi)

target_include_directories(duk.rhi
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.rhi
        PUBLIC
        # duk
        duk::macros
        duk::hash
        duk::platform
        duk::log
        duk::task
        duk::serial

        # external
        glm::glm
        volk::volk_headers
        spirv-reflect::spirv-reflect-static
        unofficial::shaderc::shaderc
        )

set_target_properties(duk.rhi PROPERTIES EXPORT_NAME rhi)

if (DUK_BUILD_TESTS)
    add_subdirectory(test)
endif ()

if (DUK_BUILD_SAMPLES)
    add_subdirectory(samples)
endif ()

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.rhi
        EXPORT duk-targets
)
