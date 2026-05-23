/// buffer_allocator.h

#ifndef DUK_RHI_BUFFER_ALLOCATOR_H
#define DUK_RHI_BUFFER_ALLOCATOR_H

#include <duk_rhi/buffer.h>
#include <duk_macros/macros.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace duk::rhi {

class CommandContext;
struct BufferCreateInfo;

struct Allocation {
    size_t offset{0};
    size_t size{0};
};

/// API-agnostic ring-allocator backed by a single RHI Buffer.
class BufferAllocator {
public:
    BufferAllocator(std::shared_ptr<Buffer> buffer, size_t alignment, uint32_t framesInFlight);

    /// Reserves \p size bytes (aligned to the device requirement) in the current frame's section.
    /// Returns an Allocation with the byte offset within the buffer and the requested size.
    /// Asserts if the section is full — call reset() at the start of each frame.
    DUK_NO_DISCARD Allocation alloc(size_t size);

    /// Advances to the next frame section. Must be called once per frame before any alloc() calls.
    void reset();

    /// Returns the underlying buffer.
    DUK_NO_DISCARD Buffer* buffer() const;

    DUK_NO_DISCARD bool valid() const;

private:
    std::shared_ptr<Buffer> m_buffer;
    size_t m_alignment{1};
    uint32_t m_framesInFlight{1};
    uint32_t m_frameIndex{0};
    size_t m_sectionSize{0};///< Per-frame capacity = buffer->size() / framesInFlight.
    size_t m_cursor{0};     ///< Current write position within the active section.
};

/// Convenience factory that handles the boilerplate of sizing, creating, and
/// mapping the backing buffer before constructing the ring-allocator.
DUK_NO_DISCARD std::shared_ptr<BufferAllocator> create_buffer_allocator(CommandContext& context, const BufferCreateInfo& bufferInfo, size_t alignment, uint32_t framesInFlight);

}// namespace duk::rhi

#endif// DUK_RHI_BUFFER_ALLOCATOR_H
