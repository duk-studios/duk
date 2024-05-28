//
// Created by rov on 1/19/2024.
//

#ifndef DUK_RENDERER_INSTANCE_BUFFER_H
#define DUK_RENDERER_INSTANCE_BUFFER_H

#include <duk_rhi/rhi.h>
#include <duk_rhi/descriptor.h>

#include <duk_objects/objects.h>

namespace duk::renderer {

struct MaterialInstanceBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    uint32_t instanceCount;
    duk::rhi::DescriptorDescription description;
};

class MaterialInstanceBuffer final {
public:
    MaterialInstanceBuffer();

    explicit MaterialInstanceBuffer(const MaterialInstanceBufferCreateInfo& instanceBufferCreateInfo);

    bool valid() const;

    duk::rhi::Descriptor descriptor() const;

    uint32_t size() const;

    uint32_t byte_size() const;

    bool expand(uint32_t instanceIndex);

    // writes to default instance data
    void write(const void* data, size_t size);

    // writes to default instance data in a specific member
    void write(uint32_t member, const void* data, size_t size);

    // writes to instance data in a specific member
    void write(const duk::objects::Id& id, const void* data, size_t size);

    // overwrites instance data in a specific member
    void write(const duk::objects::Id& id, uint32_t member, const void* data, size_t size);

    void push(const duk::objects::Id& id);

    void clear();

    void flush();

private:
    uint32_t instance_offset(const duk::objects::Id& id) const;

    bool is_valid(const duk::objects::Id& id) const;

    void update(const duk::objects::Id& id);

private:
    duk::rhi::DescriptorDescription m_description;
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    uint32_t m_bufferInstanceCount;
    std::vector<uint8_t> m_instanceData;
    std::vector<uint8_t> m_defaultInstanceData;
    std::vector<uint32_t> m_instanceVersions;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_INSTANCE_BUFFER_H
