//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_VULKAN_RENDERER_H
#define DUK_RENDERER_VULKAN_RENDERER_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/vulkan/vulkan_debug_messenger.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>
#include <duk_renderer/vulkan/vulkan_command_queue.h>
#include <duk_renderer/vulkan/vulkan_swapchain.h>

namespace duk::renderer {

struct VulkanRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
    bool hasValidationLayers;
};

class VulkanRenderer : public Renderer {
public:

    explicit VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    ~VulkanRenderer() override;

    void begin_frame() override;

    void end_frame() override;

    ExpectedCommandQueue create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) override;

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
    std::unique_ptr<VulkanSwapchain> m_swapchain;

    VulkanDebugMessenger m_debugMessenger;
    std::array<uint32_t, CommandQueueType::QUEUE_COUNT> m_queueFamilyIndices;

};

}

#endif //DUK_RENDERER_VULKAN_RENDERER_H
