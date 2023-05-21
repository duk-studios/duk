//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_VULKAN_RENDERER_H
#define DUK_RENDERER_VULKAN_RENDERER_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/pipeline/pipeline.h>
#include <duk_renderer/vulkan/vulkan_debug_messenger.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>
#include <duk_renderer/vulkan/vulkan_command_queue.h>
#include <duk_renderer/vulkan/vulkan_swapchain.h>
#include <duk_renderer/vulkan/vulkan_frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_events.h>

namespace duk::renderer {

struct VulkanRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
    uint32_t maxFramesInFlight;
    bool hasValidationLayers;
};

class VulkanRenderer : public Renderer {
public:

    explicit VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    ~VulkanRenderer() override;

    void prepare_frame() override;

    DUK_NO_DISCARD Command* acquire_image_command() override;

    DUK_NO_DISCARD Command* present_command() override;

    DUK_NO_DISCARD Image* present_image() override;

    DUK_NO_DISCARD ExpectedCommandQueue create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) override;

    DUK_NO_DISCARD ExpectedCommandScheduler create_command_scheduler() override;

    DUK_NO_DISCARD ExpectedMesh create_mesh(const MeshCreateInfo& meshCreateInfo) override;

    DUK_NO_DISCARD ExpectedShader create_shader(const ShaderCreateInfo& shaderCreateInfo) override;

    DUK_NO_DISCARD ExpectedPipeline create_pipeline(const PipelineCreateInfo& pipelineCreateInfo) override;

    DUK_NO_DISCARD ExpectedRenderPass create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) override;

    DUK_NO_DISCARD ExpectedFrameBuffer create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) override;

private:
    void create_vk_instance(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void select_vk_physical_device(uint32_t deviceIndex);

    void create_vk_surface(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void create_vk_device(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void create_vk_swapchain(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

private:

    VkInstance m_instance;
    std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
    VkSurfaceKHR m_surface;
    VkDevice m_device;
    uint32_t m_maxFramesInFlight;
    std::unique_ptr<VulkanSwapchain> m_swapchain;

    VulkanDebugMessenger m_debugMessenger;
    std::array<uint32_t, CommandQueueType::QUEUE_COUNT> m_queueFamilyIndices;
    uint32_t m_presentQueueFamilyIndex;

    uint32_t m_currentFrame;
    VulkanPrepareFrameEvent m_prepareFrameEvent;

};

}

#endif //DUK_RENDERER_VULKAN_RENDERER_H
