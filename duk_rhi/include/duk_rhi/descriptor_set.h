/// 31/05/2023
/// descriptor_set.h

#ifndef DUK_RHI_DESCRIPTOR_SET_H
#define DUK_RHI_DESCRIPTOR_SET_H

#include <duk_rhi/descriptor.h>

#include <duk_hash/hash_combine.h>

#include <cstdint>
#include <vector>

namespace duk::rhi {

class Image;
class Buffer;

class DescriptorSet {
public:
    virtual ~DescriptorSet();

    virtual void set(uint32_t binding, const Descriptor& descriptor) = 0;

    DUK_NO_DISCARD virtual uint32_t size() const noexcept = 0;

    DUK_NO_DISCARD virtual Descriptor& at(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Descriptor& at(uint32_t binding) const = 0;

    DUK_NO_DISCARD virtual Image* image(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Image* image(uint32_t binding) const = 0;

    DUK_NO_DISCARD virtual Buffer* buffer(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Buffer* buffer(uint32_t binding) const = 0;

    virtual void flush() = 0;
};

}// namespace duk::rhi

namespace std {

template<>
struct hash<duk::rhi::DescriptorDescription> {
    size_t operator()(const duk::rhi::DescriptorDescription& descriptorDescription) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorDescription.type);
        duk::hash::hash_combine(hash, descriptorDescription.moduleMask);
        return hash;
    }
};

template<>
struct hash<duk::rhi::DescriptorSetDescription> {
    size_t operator()(const duk::rhi::DescriptorSetDescription& descriptorSetDescription) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, descriptorSetDescription.bindings.begin(), descriptorSetDescription.bindings.end());
        return hash;
    }
};

}// namespace std

#endif// DUK_RHI_DESCRIPTOR_SET_H
