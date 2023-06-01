/// 20/05/2023
/// descriptor.h

#ifndef DUK_RENDERER_DESCRIPTOR_H
#define DUK_RENDERER_DESCRIPTOR_H

#include <duk_renderer/pipeline/shader.h>

#include <duk_macros/macros.h>
#include <duk_hash/hash_combine.h>

#include <vector>
#include <cstdint>

namespace duk::renderer {

enum class DescriptorType {
    UNDEFINED = 0,
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    IMAGE,
    IMAGE_SAMPLER
};

struct DescriptorDescription {
    DescriptorType type;
    Shader::Module::Mask moduleMask;
};

struct DescriptorSetDescription {
    std::vector<DescriptorDescription> bindings;
};

class Image;
class ImageSampler;
class Buffer;

class Descriptor {
public:

    Descriptor();

    Descriptor(Image* image);

    Descriptor(ImageSampler* imageSampler);

    Descriptor(Buffer* buffer);

    DUK_NO_DISCARD DescriptorType type() const;

    DUK_NO_DISCARD Image* image() const;

    DUK_NO_DISCARD ImageSampler* image_sampler() const;

private:
    union Data {
        Image* image;
        ImageSampler* imageSampler;
        Buffer* buffer;
    };

    Data m_data;
    DescriptorType m_type;
};

}

namespace std {

template<>
struct hash<duk::renderer::DescriptorDescription> {

    size_t operator()(const duk::renderer::DescriptorDescription& descriptorDescription) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorDescription.type);
        duk::hash::hash_combine(hash, descriptorDescription.moduleMask);
        return hash;
    }

};

template<>
struct hash<duk::renderer::DescriptorSetDescription> {

    size_t operator()(const duk::renderer::DescriptorSetDescription& descriptorSetDescription) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorSetDescription.bindings.begin(), descriptorSetDescription.bindings.end());
        return hash;
    }

};

}
#endif // DUK_RENDERER_DESCRIPTOR_H

