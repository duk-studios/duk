/// 20/05/2023
/// descriptor.cpp

#include <duk_renderer/pipeline/descriptor.h>
#include <duk_renderer/buffer.h>

#include <cassert>
#include <stdexcept>

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

Descriptor::Descriptor(Buffer* buffer) {
    auto type = buffer->type();
    assert(type == Buffer::Type::UNIFORM || type == Buffer::Type::STORAGE);
    m_data.buffer = buffer;
    switch (type) {
        case Buffer::Type::UNIFORM:
            m_type = DescriptorType::UNIFORM_BUFFER;
            break;
        case Buffer::Type::STORAGE:
            m_type = DescriptorType::STORAGE_BUFFER;
            break;
        default:
            throw std::invalid_argument("un-allowed Buffer::Type provided to Descriptor");
    }
}

DescriptorType Descriptor::type() const {
    return m_type;
}

Image* Descriptor::image() const {
    assert(m_type == DescriptorType::IMAGE && "descriptor is not DescriptorType::IMAGE");
    return m_data.image;
}

ImageSampler* Descriptor::image_sampler() const {
    assert(m_type == DescriptorType::IMAGE_SAMPLER && "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.imageSampler;
}

}