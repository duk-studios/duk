/// 20/05/2023
/// descriptor.h

#ifndef DUK_RHI_DESCRIPTOR_H
#define DUK_RHI_DESCRIPTOR_H

#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/image.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/sampler.h>

#include <duk_macros/macros.h>
#include <duk_hash/hash_combine.h>

#include <vector>
#include <cstdint>

namespace duk::rhi {

enum class DescriptorType {
    UNDEFINED = 0,
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    IMAGE,
    IMAGE_SAMPLER,
    STORAGE_IMAGE
};

struct DescriptorDescription {
    DescriptorType type;
    Shader::Module::Mask moduleMask;
};

struct DescriptorSetDescription {
    std::vector<DescriptorDescription> bindings;
};


class Descriptor {
public:

    Descriptor();

    static Descriptor uniform_buffer(Buffer* buffer);

    static Descriptor storage_buffer(Buffer* buffer);

    static Descriptor image(Image* image, Image::Layout layout);

    static Descriptor image_sampler(Image* image, Image::Layout layout, Sampler sampler);

    static Descriptor storage_image(Image* image, Image::Layout layout);

    DUK_NO_DISCARD DescriptorType type() const;

    DUK_NO_DISCARD duk::hash::Hash hash() const;

    DUK_NO_DISCARD Image* image() const;

    DUK_NO_DISCARD Image::Layout image_layout() const;

    DUK_NO_DISCARD Sampler sampler() const;

    DUK_NO_DISCARD Buffer* buffer() const;

    DUK_NO_DISCARD bool operator==(const Descriptor& rhs) const noexcept;

private:

    void update_hash();

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
    duk::hash::Hash m_hash;
};

}

namespace std {

template<>
struct hash<duk::rhi::DescriptorDescription> {
    size_t operator()(const duk::rhi::DescriptorDescription& descriptorDescription) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorDescription.type);
        duk::hash::hash_combine(hash, descriptorDescription.moduleMask);
        return hash;
    }
};

template<>
struct hash<duk::rhi::DescriptorSetDescription> {
    size_t operator()(const duk::rhi::DescriptorSetDescription& descriptorSetDescription) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorSetDescription.bindings.begin(), descriptorSetDescription.bindings.end());
        return hash;
    }
};

}
#endif // DUK_RHI_DESCRIPTOR_H
