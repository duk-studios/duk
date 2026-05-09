//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_VULKAN_RENDERER_H
#define DUK_RHI_VULKAN_RENDERER_H

#include <duk_rhi/rhi.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_queue.h>
#include <duk_rhi/vulkan/vulkan_rhi_capabilities.h>

namespace duk::platform {
class Window;
}

namespace duk::rhi {

struct VulkanRHICreateInfo {
    RHICreateInfo rhiCreateInfo;
    bool hasValidationLayers;
};

class VulkanRHI : public RHI {
public:
    explicit VulkanRHI(const VulkanRHICreateInfo& createInfo);

    ~VulkanRHI() override;

    DUK_NO_DISCARD RHICapabilities* capabilities() const override;

    DUK_NO_DISCARD std::shared_ptr<CommandQueue> create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) override;

private:
    void create_vk_instance(const VulkanRHICreateInfo& createInfo);

    void select_vk_physical_device(uint32_t deviceIndex);

private:
    uint32_t m_framesInFlight;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
    std::unique_ptr<VulkanRendererCapabilities> m_rendererCapabilities;
    VkDevice m_device;
    std::vector<std::shared_ptr<VulkanQueue>> m_queues;
};

}// namespace duk::rhi

#endif//DUK_RHI_VULKAN_RENDERER_H
