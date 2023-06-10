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

Descriptor::Descriptor(Image* image, Image::Layout layout) {
    m_type = DescriptorType::IMAGE;
    m_data.imageDescriptor.image = image;
    m_data.imageDescriptor.layout = layout;
}

Descriptor::Descriptor(Image* image, Image::Layout layout, Sampler sampler) {
    m_type = DescriptorType::IMAGE_SAMPLER;
    m_data.imageDescriptor.image = image;
    m_data.imageDescriptor.layout = layout;
    m_data.imageDescriptor.sampler = sampler;
}

Descriptor::Descriptor(Buffer* buffer) {
    auto type = buffer->type();
    assert(type == Buffer::Type::UNIFORM || type == Buffer::Type::STORAGE);
    m_data.bufferDescriptor.buffer = buffer;
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

Resource::HashChangedEvent& Descriptor::hash_changed_event() const {
    switch (m_type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER:
            return m_data.bufferDescriptor.buffer->hash_changed_event();
        case DescriptorType::IMAGE:
        case DescriptorType::IMAGE_SAMPLER:
            return m_data.imageDescriptor.image->hash_changed_event();
        default:
            throw std::logic_error("tried to get HashChangedEvent from an undefined descriptor");
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
            duk::hash::hash_combine(hash, m_data.bufferDescriptor.buffer);
            duk::hash::hash_combine(hash, m_data.bufferDescriptor.buffer->hash());
            break;
        case DescriptorType::IMAGE_SAMPLER:
            duk::hash::hash_combine(hash, m_data.imageDescriptor.sampler);
        case DescriptorType::IMAGE:
            duk::hash::hash_combine(hash, m_data.imageDescriptor.layout);
            duk::hash::hash_combine(hash, m_data.imageDescriptor.image);
            duk::hash::hash_combine(hash, m_data.imageDescriptor.image->hash());
            break;
        default:
            break;
    }
    return hash;
}

Image* Descriptor::image() const {
    assert(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER && "descriptor is not DescriptorType::IMAGE or DescriptorType::IMAGE_SAMPLER");
    return m_data.imageDescriptor.image;
}

Image::Layout Descriptor::image_layout() const {
    assert(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER && "descriptor is not DescriptorType::IMAGE or DescriptorType::IMAGE_SAMPLER");
    return m_data.imageDescriptor.layout;
}

Sampler Descriptor::sampler() const {
    assert(m_type == DescriptorType::IMAGE_SAMPLER && "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.imageDescriptor.sampler;
}

Buffer* Descriptor::buffer() const {
    assert((m_type == DescriptorType::UNIFORM_BUFFER || m_type == DescriptorType::STORAGE_BUFFER) && "descriptor is not DescriptorType::IMAGE_SAMPLER");
    return m_data.bufferDescriptor.buffer;
}

}