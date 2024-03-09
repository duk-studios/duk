/// 28/05/2023
/// buffer.h

#ifndef DUK_RHI_BUFFER_H
#define DUK_RHI_BUFFER_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <cstdint>
#include <duk_macros/assert.h>
#include <ranges>
#include <type_traits>

namespace duk::rhi {

class CommandQueue;

class Buffer {
public:
    enum class UpdateFrequency {
        STATIC,
        DYNAMIC
    };

    enum class Type {
        INDEX_16,
        INDEX_32,
        VERTEX,
        UNIFORM,
        STORAGE
    };

public:
    virtual ~Buffer();

    DUK_NO_DISCARD virtual const uint8_t* read_ptr(size_t offset) const = 0;

    DUK_NO_DISCARD const uint8_t* read_ptr() const;

    virtual void read(void* dst, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void read(T& dst, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        DUK_ASSERT(elementSize == element_size());
        read((void*)std::data(dst), std::size(dst) * elementSize, startElement * elementSize);
    }

    template<typename T>
    void read(T& dst, size_t elementIndex = 0) {
        constexpr size_t elementSize = sizeof(T);
        DUK_ASSERT(elementSize == element_size());
        read((void*)&dst, elementSize, elementIndex * elementSize);
    }

    DUK_NO_DISCARD virtual uint8_t* write_ptr(size_t offset) = 0;

    DUK_NO_DISCARD uint8_t* write_ptr();

    virtual void write(void* src, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void write(const T& src, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        DUK_ASSERT(elementSize == element_size());
        write((void*)std::data(src), std::size(src) * elementSize, startElement * elementSize);
    }

    template<typename T>
    void write(const T& src, size_t elementIndex = 0) {
        constexpr size_t elementSize = sizeof(T);
        DUK_ASSERT(elementSize == element_size());
        write((void*)&src, elementSize, elementIndex * elementSize);
    }

    virtual void copy_from(Buffer* srcBuffer, size_t size, size_t srcOffset, size_t dstOffset) = 0;

    /// flushes data to the GPU
    virtual void flush() = 0;

    DUK_NO_DISCARD virtual size_t element_count() const = 0;

    DUK_NO_DISCARD virtual size_t element_size() const = 0;

    DUK_NO_DISCARD virtual size_t byte_size() const = 0;

    DUK_NO_DISCARD virtual Type type() const = 0;

    DUK_NO_DISCARD virtual CommandQueue* command_queue() const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_BUFFER_H
