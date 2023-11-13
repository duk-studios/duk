/// 08/06/2023
/// vulkan_renderer_capabilities.h

#ifndef DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H
#define DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H

#include <duk_rhi/renderer_capabilities.h>

namespace duk::rhi {

class VulkanPhysicalDevice;

struct VulkanRendererCapabilitiesCreateInfo {
    VulkanPhysicalDevice* physicalDevice;
};

class VulkanRendererCapabilities : public RendererCapabilities {
public:

    explicit VulkanRendererCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo);

    ~VulkanRendererCapabilities() override;

    PixelFormat depth_format() override;

private:
    VulkanPhysicalDevice* m_physicalDevice;
};

}

#endif // DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H

