/// 20/05/2023
/// descriptor.h

#ifndef DUK_RHI_DESCRIPTOR_H
#define DUK_RHI_DESCRIPTOR_H

#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/image.h>
#include <duk_rhi/sampler.h>

#include <duk_macros/macros.h>

namespace duk::rhi {

class Descriptor {
public:
    Descriptor();

    static Descriptor uniform_buffer(Buffer* buffer);

    static Descriptor storage_buffer(Buffer* buffer);

    static Descriptor image(Image* image, Image::Layout layout);

    static Descriptor image_sampler(Image* image, Image::Layout layout, Sampler sampler);

    static Descriptor storage_image(Image* image, Image::Layout layout);

    DUK_NO_DISCARD DescriptorType type() const;

    DUK_NO_DISCARD Image* image() const;

    DUK_NO_DISCARD Image::Layout image_layout() const;

    DUK_NO_DISCARD Sampler sampler() const;

    DUK_NO_DISCARD Buffer* buffer() const;

    friend bool operator==(const Descriptor& lhs, const Descriptor& rhs);

private:
    struct ImageDescriptor {
        Image* image;
        Image::Layout layout;
        Sampler sampler;
    };

    struct BufferDescriptor {
        Buffer* buffer;
    };

    union Data {
        ImageDescriptor imageDescriptor;
        BufferDescriptor bufferDescriptor;
    };

    Data m_data;
    DescriptorType m_type;
};

bool operator==(const Descriptor& lhs, const Descriptor& rhs);

bool operator!=(const Descriptor& lhs, const Descriptor& rhs);

}// namespace duk::rhi

#endif// DUK_RHI_DESCRIPTOR_H
