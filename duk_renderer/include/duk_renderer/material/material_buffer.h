//
// Created by Ricardo on 02/05/2024.
//

#ifndef DUK_RENDERER_MATERIAL_BUFFER_H
#define DUK_RENDERER_MATERIAL_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

#include <memory>

namespace duk::renderer {

struct MaterialBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    duk::rhi::DescriptorDescription description;
};

class MaterialUniformBuffer {
public:

    MaterialUniformBuffer();

    MaterialUniformBuffer(const MaterialBufferCreateInfo& materialBufferCreateInfo);

    bool valid() const;

    duk::rhi::Descriptor descriptor() const;

    // writes the whole buffer
    void write(const void* data, uint32_t size);

    // writes a specific member of the buffer
    void write(uint32_t member, const void* data, uint32_t size);

    void flush();

private:
    duk::rhi::DescriptorDescription m_description;
    std::shared_ptr<duk::rhi::Buffer> m_buffer;
};

}

#endif //DUK_RENDERER_MATERIAL_BUFFER_H
