/// 08/06/2023
/// vulkan_renderer_capabilities.cpp

#include <duk_rhi/vulkan/vulkan_image.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_rhi_capabilities.h>

namespace duk::rhi {

VulkanRendererCapabilities::VulkanRendererCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo) : m_physicalDevice(rendererCapabilitiesCreateInfo.physicalDevice) {
}

VulkanRendererCapabilities::~VulkanRendererCapabilities() = default;

PixelFormat VulkanRendererCapabilities::depth_format() const {
    VkFormat depthFormats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return convert_pixel_format(m_physicalDevice->select_depth_format(depthFormats));
}

bool VulkanRendererCapabilities::is_format_supported(PixelFormat format, Image::Usage usage) const {
    auto vkFormat = convert_pixel_format(format);
    return m_physicalDevice->is_format_supported(vkFormat, VK_IMAGE_TILING_OPTIMAL, usage_format_features(usage));
}

}// namespace duk::rhi