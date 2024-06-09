//
// Created by Ricardo on 09/06/2024.
//

#include <duk_renderer/mesh/draw_buffer.h>

namespace duk::renderer {

namespace detail {

static constexpr size_t kBufferChunkSize = 4096;

}

DrawBuffer::DrawBuffer(const DrawBufferCreateInfo& drawBufferCreateInfo)
    : m_rhi(drawBufferCreateInfo.rhi)
    , m_commandQueue(drawBufferCreateInfo.commandQueue)
    , m_capacity(0)
    , m_size(0) {
    expand(detail::kBufferChunkSize);
}

void DrawBuffer::push_draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    constexpr auto kCommandSize = sizeof(IndirectDrawCommand);

    expand(m_size + kCommandSize);

    const IndirectDrawCommand command = {.vertexCount = vertexCount, .instanceCount = instanceCount, .firstVertex = firstVertex, .firstInstance = firstInstance};

    m_buffer->write(&command, kCommandSize, m_size);
    m_size += kCommandSize;
}

void DrawBuffer::push_indexed_draw(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    constexpr auto kCommandSize = sizeof(IndirectDrawIndexedCommand);

    expand(m_size + kCommandSize);

    const IndirectDrawIndexedCommand command = {
            .indexCount = indexCount,
            .instanceCount = instanceCount,
            .firstIndex = firstIndex,
            .vertexOffset = vertexOffset,
            .firstInstance = firstInstance,
    };

    m_buffer->write(&command, kCommandSize, m_size);
    m_size += kCommandSize;
}

void DrawBuffer::flush() {
    m_buffer->flush();
}

void DrawBuffer::clear() {
    m_size = 0;
}

size_t DrawBuffer::size() const {
    return m_size;
}

const duk::rhi::Buffer* DrawBuffer::buffer() const {
    return m_buffer.get();
}

void DrawBuffer::expand(size_t capacity) {
    if (capacity < m_capacity) {
        return;
    }

    capacity = (capacity / detail::kBufferChunkSize + 1) * detail::kBufferChunkSize;

    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.type = duk::rhi::Buffer::Type::INDIRECT;
    bufferCreateInfo.commandQueue = m_commandQueue;
    bufferCreateInfo.elementCount = 1;
    bufferCreateInfo.elementSize = capacity;
    bufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;

    auto buffer = m_rhi->create_buffer(bufferCreateInfo);

    if (m_buffer && m_size > 0) {
        buffer->copy_from(m_buffer.get(), m_size, 0, 0);
    }
    std::swap(m_buffer, buffer);
    m_capacity = capacity;
}
}// namespace duk::renderer
