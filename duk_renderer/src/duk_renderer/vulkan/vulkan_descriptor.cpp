/// 20/05/2023
/// vulkan_descriptor.cpp

#include <duk_renderer/vulkan/vulkan_descriptor.h>

#include <stdexcept>

namespace duk::renderer {

namespace vk {
VkDescriptorType convert_descriptor_type(DescriptorType descriptorType) {
    VkDescriptorType converted;
    switch(descriptorType) {
        case DescriptorType::UNIFORM_BUFFER: converted = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
        case DescriptorType::STORAGE_BUFFER: converted = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
        case DescriptorType::IMAGE: converted = VK_DESCRIPTOR_TYPE_SAMPLER; break;
        case DescriptorType::IMAGE_SAMPLER: converted = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
        default:
            throw std::logic_error("tried to convert a unsupported DescriptorType");
    }
    return converted;
}
}

}