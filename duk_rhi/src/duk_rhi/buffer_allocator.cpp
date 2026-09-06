/// buffer_allocator.cpp

#include <duk_rhi/buffer_allocator.h>
#include <duk_rhi/command_context.h>

#include <duk_macros/assert.h>

namespace duk::rhi {

BufferAllocator::BufferAllocator(CommandContext& context, const BufferAllocatorCreateInfo& createInfo)
    : m_buffer(nullptr)
    , m_alignment(createInfo.alignment)
    , m_framesInFlight(createInfo.framesInFlight)
    , m_frameIndex(0)
    , m_sectionSize(0)
    , m_cursor(0) {
    const size_t effectiveAlignment = createInfo.alignment > 0 ? createInfo.alignment : 1;
    const size_t sectionSize = (createInfo.size + effectiveAlignment - 1) & ~(effectiveAlignment - 1);

    BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.properties = createInfo.properties;
    bufferCreateInfo.type = createInfo.type;
    bufferCreateInfo.size = sectionSize * createInfo.framesInFlight;

    m_buffer = context.create_buffer(bufferCreateInfo);
    m_sectionSize = sectionSize;
}

std::optional<BufferAllocation> BufferAllocator::alloc(size_t size) {
    const size_t aligned = (m_cursor + m_alignment - 1) & ~(m_alignment - 1);
    const size_t sectionEnd = static_cast<size_t>(m_frameIndex) * m_sectionSize + m_sectionSize;
    if (aligned + size > sectionEnd) {
        return std::nullopt;
    }
    m_cursor = aligned + size;
    return BufferAllocation{aligned, size};
}

void BufferAllocator::reset() {
    m_frameIndex = (m_frameIndex + 1) % m_framesInFlight;
    m_cursor = static_cast<size_t>(m_frameIndex) * m_sectionSize;
}

Buffer* BufferAllocator::buffer() const {
    return m_buffer.get();
}

bool BufferAllocator::valid() const {
    return m_buffer != nullptr;
}

size_t BufferAllocator::remaining() const {
    const size_t aligned = (m_cursor + m_alignment - 1) & ~(m_alignment - 1);
    const size_t sectionEnd = static_cast<size_t>(m_frameIndex) * m_sectionSize + m_sectionSize;
    return sectionEnd - aligned;
}

size_t BufferAllocator::capacity() const {
    return m_sectionSize;
}

}// namespace duk::rhi
