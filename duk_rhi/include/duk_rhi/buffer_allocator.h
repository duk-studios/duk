/// buffer_allocator.h

#ifndef DUK_RHI_BUFFER_ALLOCATOR_H
#define DUK_RHI_BUFFER_ALLOCATOR_H

#include <duk_rhi/buffer.h>
#include <duk_macros/macros.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

namespace duk::rhi {

class CommandContext;

struct BufferAllocatorCreateInfo {
    Buffer::Type type;
    Buffer::UpdateFrequency updateFrequency;
    size_t size;
    size_t alignment;
    uint32_t framesInFlight;
};

struct BufferAllocation {
    size_t offset{0};
    size_t size{0};
};

/// API-agnostic ring-allocator backed by a single RHI Buffer.
class BufferAllocator {
public:

    BufferAllocator(CommandContext& context, const BufferAllocatorCreateInfo& createInfo);

    /// Reserves \p size bytes (aligned to the device requirement) in the current frame's section.
    /// Returns an Allocation with the byte offset within the buffer and the requested size.
    /// Asserts if the section is full — call reset() at the start of each frame.
    DUK_NO_DISCARD std::optional<BufferAllocation> alloc(size_t size);

    /// Advances to the next frame section. Must be called once per frame before any alloc() calls.
    void reset();

    /// Returns the underlying buffer.
    DUK_NO_DISCARD Buffer* buffer() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD size_t remaining() const;

    DUK_NO_DISCARD size_t capacity() const;

private:
    std::shared_ptr<Buffer> m_buffer;
    size_t m_alignment{1};
    uint32_t m_framesInFlight{1};
    uint32_t m_frameIndex{0};
    size_t m_sectionSize{0};
    size_t m_cursor{0};
};

}// namespace duk::rhi

#endif// DUK_RHI_BUFFER_ALLOCATOR_H
