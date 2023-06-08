/// 08/06/2023
/// vulkan_renderer_capabilities.h

#ifndef DUK_RENDERER_VULKAN_RENDERER_CAPABILITIES_H
#define DUK_RENDERER_VULKAN_RENDERER_CAPABILITIES_H

#include <duk_renderer/renderer_capabilities.h>

namespace duk::renderer {

class VulkanPhysicalDevice;

struct VulkanRendererCapabilitiesCreateInfo {
    VulkanPhysicalDevice* physicalDevice;
};

class VulkanRendererCapabilities : public RendererCapabilities {
public:

    explicit VulkanRendererCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo);

    ~VulkanRendererCapabilities() override;

    Image::PixelFormat depth_format() override;

private:
    VulkanPhysicalDevice* m_physicalDevice;
};

}

#endif // DUK_RENDERER_VULKAN_RENDERER_CAPABILITIES_H

