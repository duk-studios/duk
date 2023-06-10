/// 28/05/2023
/// buffer.h

#ifndef DUK_RENDERER_BUFFER_H
#define DUK_RENDERER_BUFFER_H

#include <duk_renderer/resource.h>
#include <duk_macros/macros.h>
#include <duk_hash/hash.h>

#include <cstdint>
#include <cassert>
#include <ranges>
#include <type_traits>

namespace duk::renderer {

class Buffer : public Resource {
public:
    enum class UpdateFrequency {
        STATIC,
        DYNAMIC,
        SHARED
    };

    enum class Type {
        INDEX_16,
        INDEX_32,
        VERTEX,
        UNIFORM,
        STORAGE
    };

public:

    virtual void read(void* dst, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void read(T& dst, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        assert(elementSize == element_size());
        read((void*)std::data(dst), std::size(dst) * elementSize, startElement * elementSize);
    }

    template<typename T>
    void read(T& dst, size_t elementIndex = 0) {
        constexpr size_t elementSize = sizeof(T);
        assert(elementSize == element_size());
        read((void*)&dst, elementSize, elementIndex * elementSize);
    }

    virtual void write(void* src, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void write(const T& src, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        assert(elementSize == element_size());
        write((void*)std::data(src), std::size(src) * elementSize, startElement * elementSize);
    }

    template<typename T>
    void write(const T& src, size_t elementIndex = 0) {
        constexpr size_t elementSize = sizeof(T);
        assert(elementSize == element_size());
        write((void*)&src, elementSize, elementIndex * elementSize);
    }

    /// Makes recent writes available to the GPU
    virtual void flush() = 0;

    /// Invalidates CPU cache and fetches data from GPU
    virtual void invalidate() = 0;

    DUK_NO_DISCARD virtual size_t element_count() const = 0;

    DUK_NO_DISCARD virtual size_t element_size() const = 0;

    DUK_NO_DISCARD virtual size_t byte_size() const = 0;

    DUK_NO_DISCARD virtual Type type() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;

};

}

#endif // DUK_RENDERER_BUFFER_H

