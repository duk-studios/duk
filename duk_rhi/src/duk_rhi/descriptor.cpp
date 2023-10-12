/// 20/05/2023
/// descriptor.cpp

#include <duk_rhi/descriptor.h>
#include <duk_rhi/buffer.h>

#include <cassert>
#include <stdexcept>

namespace duk::rhi {

Descriptor Descriptor::uniform_buffer(Buffer* buffer) {
    assert(buffer->type() == Buffer::Type::UNIFORM);
    Descriptor descriptor;
    descriptor.m_data.bufferDescriptor.buffer = buffer;
    descriptor.m_type = DescriptorType::UNIFORM_BUFFER;
    descriptor.update_hash();
    return descriptor;
}

Descriptor Descriptor::storage_buffer(Buffer* buffer) {
    assert(buffer->type() == Buffer::Type::STORAGE);
    Descriptor descriptor;
    descriptor.m_data.bufferDescriptor.buffer = buffer;
    descriptor.m_type = DescriptorType::STORAGE_BUFFER;
    descriptor.update_hash();
    return descriptor;
}

Descriptor Descriptor::image(Image* image, Image::Layout layout) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_type = DescriptorType::IMAGE;
    descriptor.update_hash();
    return descriptor;
}

Descriptor Descriptor::image_sampler(Image* image, Image::Layout layout, Sampler sampler) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_data.imageDescriptor.sampler = sampler;
    descriptor.m_type = DescriptorType::IMAGE_SAMPLER;
    descriptor.update_hash();
    return descriptor;
}

Descriptor Descriptor::storage_image(Image* image, Image::Layout layout) {
    Descriptor descriptor;
    descriptor.m_data.imageDescriptor.image = image;
    descriptor.m_data.imageDescriptor.layout = layout;
    descriptor.m_type = DescriptorType::STORAGE_IMAGE;
    descriptor.update_hash();
    return descriptor;
}

Descriptor::Descriptor() {
    m_type = DescriptorType::UNDEFINED;
    m_hash = 0;
}

DescriptorType Descriptor::type() const {
    return m_type;
}

duk::hash::Hash Descriptor::hash() const {
    return m_hash;
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

bool Descriptor::operator==(const Descriptor& rhs) const noexcept {
    return m_hash == rhs.m_hash;
}

void Descriptor::update_hash() {
    duk::hash::Hash hash = 0;
    switch (m_type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER:
            duk::hash::hash_combine(hash, m_data.bufferDescriptor.buffer);
            break;
        case DescriptorType::IMAGE_SAMPLER:
            duk::hash::hash_combine(hash, m_data.imageDescriptor.sampler);
        case DescriptorType::IMAGE:
        case DescriptorType::STORAGE_IMAGE:
            duk::hash::hash_combine(hash, m_data.imageDescriptor.layout);
            duk::hash::hash_combine(hash, m_data.imageDescriptor.image);
            break;
        default:
            break;
    }
    m_hash = hash;
}

}