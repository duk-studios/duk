/// 28/05/2023
/// buffer.h

#ifndef DUK_RHI_BUFFER_H
#define DUK_RHI_BUFFER_H

#include <duk_macros/macros.h>
#include <duk_macros/assert.h>

#include <cstdint>
#include <cstring>

namespace duk::rhi {

class Buffer {
public:
    enum class UpdateFrequency {
        STATIC,///< Device-local; written via staging through the command context.
        DYNAMIC///< Host-visible; written directly by the CPU through the command context.
    };

    enum class Type {
        INDEX,
        VERTEX,
        UNIFORM,
        STORAGE,
        INDIRECT
    };

    virtual ~Buffer() = default;

    /// Return the requested size of the buffer in bytes. The actual allocated size may be larger.
    DUK_NO_DISCARD virtual size_t size() const = 0;

    /// Returns the currently mapped base pointer, or nullptr if not mapped.
    DUK_NO_DISCARD virtual void* data() const = 0;

    void write(const void* src, size_t offset, size_t size) const;

    void read(void* dst, size_t offset, size_t size) const;

    template<typename T>
    void write(const T& src, size_t offset = 0) const;

    template<typename T>
    void read(T& dst, size_t offset = 0) const;
};

inline void Buffer::write(const void* src, size_t offset, size_t size) const {
    DUK_ASSERT(data() != nullptr && "Buffer must be mapped before writing");
    DUK_ASSERT(offset + size <= this->size() && "Write range exceeds buffer size");
    std::memcpy(static_cast<uint8_t*>(data()) + offset, src, size);
}

inline void Buffer::read(void* dst, size_t offset, size_t size) const {
    DUK_ASSERT(data() != nullptr && "Buffer must be mapped before reading");
    DUK_ASSERT(offset + size <= this->size() && "Read range exceeds buffer size");
    std::memcpy(dst, static_cast<const uint8_t*>(data()) + offset, size);
}

template<typename T>
void Buffer::write(const T& src, size_t offset) const {
    write(&src, offset, sizeof(T));
}

template<typename T>
void Buffer::read(T& dst, size_t offset) const {
    read(&dst, offset, sizeof(T));
}

}// namespace duk::rhi

#endif// DUK_RHI_BUFFER_H
