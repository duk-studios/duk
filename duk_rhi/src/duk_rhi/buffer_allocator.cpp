/// buffer_allocator.cpp

#include <duk_rhi/buffer_allocator.h>
#include <duk_rhi/command_context.h>

#include <duk_macros/assert.h>

namespace duk::rhi {

BufferAllocator::BufferAllocator(std::shared_ptr<Buffer> buffer, size_t alignment, uint32_t framesInFlight)
    : m_buffer(std::move(buffer))
    , m_alignment(alignment > 0 ? alignment : 1)
    , m_framesInFlight(framesInFlight > 0 ? framesInFlight : 1)
    , m_frameIndex(0)
    , m_sectionSize(m_buffer->size() / m_framesInFlight)
    , m_cursor(0) {
    DUK_ASSERT(m_buffer != nullptr);
    DUK_ASSERT(m_sectionSize > 0);
}

Allocation BufferAllocator::alloc(size_t size) {
    const size_t aligned    = (m_cursor + m_alignment - 1) & ~(m_alignment - 1);
    const size_t sectionEnd = static_cast<size_t>(m_frameIndex) * m_sectionSize + m_sectionSize;
    DUK_ASSERT(aligned + size <= sectionEnd);
    m_cursor = aligned + size;
    return Allocation{aligned, size};
}

void BufferAllocator::reset() {
    m_frameIndex = (m_frameIndex + 1) % m_framesInFlight;
    m_cursor     = static_cast<size_t>(m_frameIndex) * m_sectionSize;
}

Buffer* BufferAllocator::buffer() const {
    return m_buffer.get();
}

bool BufferAllocator::valid() const {
    return m_buffer != nullptr;
}

std::shared_ptr<BufferAllocator> create_buffer_allocator(
    CommandContext& context,
    const BufferCreateInfo& bufferInfo,
    size_t alignment,
    uint32_t framesInFlight) {
    const size_t effectiveAlignment = alignment > 0 ? alignment : 1;
    const size_t sectionSize = (bufferInfo.size + effectiveAlignment - 1) & ~(effectiveAlignment - 1);

    BufferCreateInfo actualInfo = bufferInfo;
    actualInfo.size = sectionSize * framesInFlight;

    return std::make_shared<BufferAllocator>(context.create_buffer(actualInfo), effectiveAlignment, framesInFlight);
}

}// namespace duk::rhi
