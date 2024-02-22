/// 17/02/2024
/// material_descriptor_set.cpp

#include <duk_renderer/resources/materials/material_descriptor_set.h>

namespace duk::renderer {

MaterialDescriptorSet::ResourceView<ImageResource> MaterialDescriptorSet::images() {
    return MaterialDescriptorSet::ResourceView<ImageResource>(*this);
}

ImageResource& MaterialDescriptorSet::image_at(uint32_t index) {
    throw std::invalid_argument("no ImageResource available in this MaterialDescriptorSet");
}

bool MaterialDescriptorSet::is_image(uint32_t index) {
    return false;
}

InstanceBuffer* MaterialDescriptorSet::instance_buffer() {
    return nullptr;
}

}
