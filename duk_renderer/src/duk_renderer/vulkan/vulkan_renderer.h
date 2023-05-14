//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_VULKAN_RENDERER_H
#define DUK_RENDERER_VULKAN_RENDERER_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/vulkan/vulkan_debug_messenger.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>
#include <duk_renderer/vulkan/vulkan_queue.h>

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

private:
    void create_vk_instance(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void select_vk_physical_device(VkInstance instance, uint32_t deviceIndex);

    void create_vk_surface(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void create_vk_device(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

private:

    VkInstance m_instance;
    std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
    VkSurfaceKHR m_surface;
    VkDevice m_device;

    VulkanDebugMessenger m_debugMessenger;

};

}

#endif //DUK_RENDERER_VULKAN_RENDERER_H
