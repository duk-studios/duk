/// 20/05/2023
/// descriptor.cpp

#include <duk_renderer/descriptor.h>
#include <duk_renderer/buffer.h>

#include <cassert>
#include <stdexcept>

namespace duk::renderer {

Descriptor Descriptor::uniform_buffer(Buffer* buffer) {
    assert(buffer->type() == Buffer::Type::UNIFORM);
    Descriptor descriptor;
    descriptor.m_data.bufferDescriptor.buffer = buffer;
    descriptor.m_type = DescriptorType::UNIFORM_BUFFER;
    return descriptor;
}

Descriptor Descriptor::storage_buffer(Buffer* buffer) {
    assert(buffer->type() == Buffer::Type::STORAGE);
    Descriptor descriptor;
    descriptor.m_data.bufferDescriptor.buffer = buffer;
    descriptor.m_type = DescriptorType::STORAGE_BUFFER;
    return descriptor;
}

Descriptor Descriptor::image(Image* image, Image::Layout layout) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_type = DescriptorType::IMAGE;
    return descriptor;
}

Descriptor Descriptor::image_sampler(Image* image, Image::Layout layout, Sampler sampler) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_data.imageDescriptor.sampler = sampler;
    descriptor.m_type = DescriptorType::IMAGE_SAMPLER;
    return descriptor;
}

Descriptor Descriptor::storage_image(Image* image, Image::Layout layout) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_type = DescriptorType::STORAGE_IMAGE;
    return descriptor;
}

Descriptor::Descriptor() {
    m_type = DescriptorType::UNDEFINED;
}

Resource::HashChangedEvent& Descriptor::hash_changed_event() const {
    switch (m_type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER:
            return m_data.bufferDescriptor.buffer->hash_changed_event();
        case DescriptorType::IMAGE:
        case DescriptorType::IMAGE_SAMPLER:
        case DescriptorType::STORAGE_IMAGE:
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
        case DescriptorType::STORAGE_IMAGE:
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
    assert(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER  || m_type == DescriptorType::STORAGE_IMAGE);
    return m_data.imageDescriptor.image;
}

Image::Layout Descriptor::image_layout() const {
    assert(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER  || m_type == DescriptorType::STORAGE_IMAGE);
    return m_data.imageDescriptor.layout;
}

Sampler Descriptor::sampler() const {
    assert(m_type == DescriptorType::IMAGE_SAMPLER);
    return m_data.imageDescriptor.sampler;
}

Buffer* Descriptor::buffer() const {
    assert(m_type == DescriptorType::UNIFORM_BUFFER || m_type == DescriptorType::STORAGE_BUFFER);
    return m_data.bufferDescriptor.buffer;
}

}