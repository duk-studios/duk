//
// Created by Ricardo on 09/06/2024.
//

#ifndef DUK_RENDERER_DRAW_BUFFER_H
#define DUK_RENDERER_DRAW_BUFFER_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class MeshBuffer;

struct IndirectDrawCommand {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

struct IndirectDrawIndexedCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

struct DrawBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class DrawBuffer {
public:
    explicit DrawBuffer(const DrawBufferCreateInfo& drawBufferCreateInfo);

    void push_draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

    void push_indexed_draw(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

    void flush();

    void clear();

    size_t size() const;

    const duk::rhi::Buffer* buffer() const;

private:
    void expand(size_t capacity);

private:
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    std::shared_ptr<rhi::Buffer> m_buffer;
    size_t m_capacity;
    size_t m_size;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_DRAW_BUFFER_H
