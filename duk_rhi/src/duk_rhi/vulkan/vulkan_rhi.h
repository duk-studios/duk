//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_VULKAN_RENDERER_H
#define DUK_RHI_VULKAN_RENDERER_H

#include <duk_rhi/rhi.h>
#include <duk_rhi/vulkan/vulkan_debug_messenger.h>
#include <duk_rhi/vulkan/vulkan_rhi_capabilities.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/command/vulkan_command_queue.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>
#include <duk_rhi/vulkan/vulkan_frame_buffer.h>
#include <duk_rhi/vulkan/vulkan_events.h>
#include <duk_rhi/vulkan/vulkan_resource_manager.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>

namespace duk::rhi {

struct VulkanRHICreateInfo {
    RHICreateInfo renderHardwareInterfaceCreateInfo;
    uint32_t maxFramesInFlight;
    bool hasValidationLayers;
};

class VulkanRHI : public RHI {
public:

    explicit VulkanRHI(const VulkanRHICreateInfo& vulkanRendererCreateInfo);

    ~VulkanRHI() override;

    void prepare_frame() override;

    DUK_NO_DISCARD Command* acquire_image_command() override;

    DUK_NO_DISCARD Command* present_command() override;

    DUK_NO_DISCARD Image* present_image() override;

    DUK_NO_DISCARD RHICapabilities* capabilities() const override;

    DUK_NO_DISCARD std::shared_ptr<CommandQueue> create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<CommandScheduler> create_command_scheduler() override;

    DUK_NO_DISCARD std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<GraphicsPipeline> create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<ComputePipeline> create_compute_pipeline(const ComputePipelineCreateInfo& pipelineCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<RenderPass> create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<Buffer> create_buffer(const BufferCreateInfo& bufferCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<Image> create_image(const ImageCreateInfo& imageCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<DescriptorSet> create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) override;

    DUK_NO_DISCARD std::shared_ptr<FrameBuffer> create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) override;

private:
    void create_vk_instance(const VulkanRHICreateInfo& vulkanRendererCreateInfo);

    void select_vk_physical_device(uint32_t deviceIndex);

    void create_vk_surface(const VulkanRHICreateInfo& vulkanRendererCreateInfo);

    void create_vk_device(const VulkanRHICreateInfo& vulkanRendererCreateInfo);

    void create_vk_swapchain(const VulkanRHICreateInfo& vulkanRendererCreateInfo);

    void create_resource_manager();

    void create_descriptor_set_layout_cache();

    void create_sampler_cache();

private:

    VkInstance m_instance;
    std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
    std::unique_ptr<VulkanRendererCapabilities> m_rendererCapabilities;
    VkSurfaceKHR m_surface;
    VkDevice m_device;
    uint32_t m_maxFramesInFlight;
    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanResourceManager> m_resourceManager;
    std::unique_ptr<VulkanDescriptorSetLayoutCache> m_descriptorSetLayoutCache;
    std::unique_ptr<VulkanSamplerCache> m_samplerCache;

    VulkanDebugMessenger m_debugMessenger;
    std::unordered_map<CommandQueue::Type::Mask, uint32_t> m_queueFamilyIndices;

    uint32_t m_currentFrame;
    VulkanPrepareFrameEvent m_prepareFrameEvent;

};

}

#endif //DUK_RHI_VULKAN_RENDERER_H
