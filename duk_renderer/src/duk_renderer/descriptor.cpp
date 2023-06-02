/// 20/05/2023
/// descriptor.cpp

#include <duk_renderer/descriptor.h>
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

duk::hash::Hash Descriptor::hash() const {
    duk::hash::Hash hash = 0;
    switch (m_type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER:
            duk::hash::hash_combine(hash, m_data.buffer);
            break;
        case DescriptorType::IMAGE:
        case DescriptorType::IMAGE_SAMPLER:
            duk::hash::hash_combine(hash, m_data.image);
            break;
        default:
            break;
    }
    return hash;
}

Image* Descriptor::image() const {
    assert(m_type == DescriptorType::IMAGE && "descriptor is not DescriptorType::IMAGE");
    return m_data.image;
}

ImageSampler* Descriptor::image_sampler() const {
    assert(m_type == DescriptorType::IMAGE_SAMPLER && "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.imageSampler;
}

Buffer* Descriptor::buffer() const {
    assert((m_type == DescriptorType::UNIFORM_BUFFER || m_type == DescriptorType::STORAGE_BUFFER) && "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.buffer;
}
}