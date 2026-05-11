/// 08/06/2023
/// vulkan_renderer_capabilities.cpp

#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_capabilities.h>

namespace duk::rhi {

VulkanCapabilities::VulkanCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo)
    : m_physicalDevice(rendererCapabilitiesCreateInfo.physicalDevice) {
}

VulkanCapabilities::~VulkanCapabilities() = default;

PixelFormat VulkanCapabilities::depth_format() const {
    VkFormat depthFormats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return convert_pixel_format(m_physicalDevice->select_depth_format(depthFormats));
}

bool VulkanCapabilities::is_format_supported(PixelFormat format, Image::Usage usage) const {
    auto vkFormat = convert_pixel_format(format);
    return m_physicalDevice->is_format_supported(vkFormat, VK_IMAGE_TILING_OPTIMAL, usage_format_features(usage));
}

bool VulkanCapabilities::is_multi_draw_indirect_supported() const {
    return m_physicalDevice->features().multiDrawIndirect;
}

}// namespace duk::rhi