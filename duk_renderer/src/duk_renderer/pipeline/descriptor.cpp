/// 20/05/2023
/// descriptor.cpp

#include <duk_renderer/pipeline/descriptor.h>
#include <duk_macros/assert.h>

namespace duk::renderer {

Descriptor::Descriptor() {
    m_type = DescriptorType::UNDEFINED;
}

Descriptor::Descriptor(Image* image) {
    m_type = DescriptorType::IMAGE;
    m_data.image = image;
}

Descriptor::Descriptor(ImageSampler* imageSampler) {
    m_type = DescriptorType::IMAGE_SAMPLER;
    m_data.imageSampler = imageSampler;
}

DescriptorType Descriptor::type() const {
    return m_type;
}

Image* Descriptor::image() const {
    DUK_ASSERT(m_type == DescriptorType::IMAGE, "descriptor is not DescriptorType::IMAGE");
    return m_data.image;
}

ImageSampler* Descriptor::image_sampler() const {
    DUK_ASSERT(m_type == DescriptorType::IMAGE_SAMPLER, "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.imageSampler;
}

}