/// 08/06/2023
/// vulkan_renderer_capabilities.h

#ifndef DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H
#define DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H

#include <duk_rhi/rhi_capabilities.h>

namespace duk::rhi {

class VulkanPhysicalDevice;

struct VulkanRendererCapabilitiesCreateInfo {
    VulkanPhysicalDevice* physicalDevice;
};

class VulkanRendererCapabilities : public RHICapabilities {
public:

    explicit VulkanRendererCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo);

    ~VulkanRendererCapabilities() override;

    DUK_NO_DISCARD PixelFormat depth_format() const override;

    DUK_NO_DISCARD bool is_format_supported(PixelFormat format, Image::Usage usage) const override;

private:
    VulkanPhysicalDevice* m_physicalDevice;
};

}

#endif // DUK_RHI_VULKAN_RENDERER_CAPABILITIES_H

