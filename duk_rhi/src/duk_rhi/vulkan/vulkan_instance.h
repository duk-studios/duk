//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_VULKAN_INSTANCE_H
#define DUK_RHI_VULKAN_INSTANCE_H

#include <duk_rhi/capabilities.h>
#include <duk_rhi/instance.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_queue.h>

namespace duk::platform {
class Window;
}

namespace duk::rhi {

struct VulkanRHICreateInfo {
    InstanceCreateInfo rhiCreateInfo;
    bool hasValidationLayers;
};

class VulkanInstance : public Instance {
public:
    explicit VulkanInstance(const VulkanRHICreateInfo& createInfo);

    ~VulkanInstance() override;

    DUK_NO_DISCARD const Capabilities& capabilities() const override;

    DUK_NO_DISCARD std::unique_ptr<CommandContext> create_command_context(const CommandContextCreateInfo& commandContextCreateInfo) override;

    DUK_NO_DISCARD std::unique_ptr<CommandQueue> create_command_queue(const CommandContextCreateInfo& commandContextCreateInfo) override;

    DUK_NO_DISCARD std::unique_lock<std::shared_mutex> unique_device_lock();

    DUK_NO_DISCARD std::shared_lock<std::shared_mutex> shared_device_lock();

    VkResult wait_idle() const;

    VkInstance handle() const;

    VkDevice device() const;

private:
    void create_vk_instance(const VulkanRHICreateInfo& createInfo);

    void select_vk_physical_device(uint32_t deviceIndex);

private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
    Capabilities m_capabilities;
    VkDevice m_device;
    std::vector<std::shared_ptr<VulkanQueue>> m_queues;
    std::shared_mutex m_deviceMutex;
};

}// namespace duk::rhi

#endif//DUK_RHI_VULKAN_INSTANCE_H
