/// 20/05/2023
/// descriptor.cpp

#include <duk_rhi/buffer.h>
#include <duk_rhi/descriptor.h>

#include <duk_macros/assert.h>
#include <stdexcept>

namespace duk::rhi {

Descriptor Descriptor::uniform_buffer(Buffer* buffer) {
    DUK_ASSERT(buffer->type() == Buffer::Type::UNIFORM);
    Descriptor descriptor;
    descriptor.m_data.bufferDescriptor.buffer = buffer;
    descriptor.m_type = DescriptorType::UNIFORM_BUFFER;
    return descriptor;
}

Descriptor Descriptor::storage_buffer(Buffer* buffer) {
    DUK_ASSERT(buffer->type() == Buffer::Type::STORAGE);
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

Descriptor::Descriptor()
    : m_data({})
    , m_type(DescriptorType::UNDEFINED) {
}

DescriptorType Descriptor::type() const {
    return m_type;
}

Image* Descriptor::image() const {
    DUK_ASSERT(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER || m_type == DescriptorType::STORAGE_IMAGE);
    return m_data.imageDescriptor.image;
}

Image::Layout Descriptor::image_layout() const {
    DUK_ASSERT(m_type == DescriptorType::IMAGE || m_type == DescriptorType::IMAGE_SAMPLER || m_type == DescriptorType::STORAGE_IMAGE);
    return m_data.imageDescriptor.layout;
}

Sampler Descriptor::sampler() const {
    DUK_ASSERT(m_type == DescriptorType::IMAGE_SAMPLER);
    return m_data.imageDescriptor.sampler;
}

Buffer* Descriptor::buffer() const {
    DUK_ASSERT(m_type == DescriptorType::UNIFORM_BUFFER || m_type == DescriptorType::STORAGE_BUFFER);
    return m_data.bufferDescriptor.buffer;
}

bool operator==(const Descriptor& lhs, const Descriptor& rhs) {
    if (lhs.m_type != rhs.m_type) {
        return false;
    }
    switch (lhs.m_type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER:
            return lhs.m_data.bufferDescriptor.buffer == rhs.m_data.bufferDescriptor.buffer;
        case DescriptorType::IMAGE_SAMPLER:
            return lhs.m_data.imageDescriptor.image == rhs.m_data.imageDescriptor.image && lhs.m_data.imageDescriptor.sampler == rhs.m_data.imageDescriptor.sampler && lhs.m_data.imageDescriptor.layout == rhs.m_data.imageDescriptor.layout;
        case DescriptorType::IMAGE:
            return lhs.m_data.imageDescriptor.image == rhs.m_data.imageDescriptor.image;
        case DescriptorType::STORAGE_IMAGE:
            return lhs.m_data.imageDescriptor.image == rhs.m_data.imageDescriptor.image;
        default:
            return true;
    }
}

bool operator!=(const Descriptor& lhs, const Descriptor& rhs) {
    return !(lhs == rhs);
}

}// namespace duk::rhi