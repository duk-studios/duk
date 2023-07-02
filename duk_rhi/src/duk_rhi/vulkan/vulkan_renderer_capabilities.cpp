/// 08/06/2023
/// vulkan_renderer_capabilities.cpp

#include <duk_rhi/vulkan/vulkan_renderer_capabilities.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_image.h>

namespace duk::rhi {

VulkanRendererCapabilities::VulkanRendererCapabilities(const VulkanRendererCapabilitiesCreateInfo& rendererCapabilitiesCreateInfo) :
    m_physicalDevice(rendererCapabilitiesCreateInfo.physicalDevice) {

}

VulkanRendererCapabilities::~VulkanRendererCapabilities() = default;

Image::PixelFormat VulkanRendererCapabilities::depth_format() {
    VkFormat depthFormats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return convert_pixel_format(m_physicalDevice->select_depth_format(depthFormats));
}

}