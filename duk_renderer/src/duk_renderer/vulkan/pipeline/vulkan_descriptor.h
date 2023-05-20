/// 20/05/2023
/// vulkan_descriptor.h

#ifndef DUK_RENDERER_VULKAN_DESCRIPTOR_H
#define DUK_RENDERER_VULKAN_DESCRIPTOR_H

#include <duk_renderer/pipeline/descriptor.h>
#include <duk_renderer/vulkan/vulkan_import.h>

namespace duk::renderer {

namespace vk {

VkDescriptorType convert_descriptor_type(DescriptorType descriptorType);

}

}

#endif // DUK_RENDERER_VULKAN_DESCRIPTOR_H

