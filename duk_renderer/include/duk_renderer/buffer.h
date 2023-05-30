/// 28/05/2023
/// buffer.h

#ifndef DUK_RENDERER_BUFFER_H
#define DUK_RENDERER_BUFFER_H

#include <duk_macros/macros.h>

#include <cstdint>
#include <cassert>
#include <ranges>
#include <type_traits>

namespace duk::renderer {

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

    virtual ~Buffer() = default;

    virtual void read(void* dst, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void read(T& dst, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        assert(elementSize == element_size());
        read((void*)std::data(dst), std::size(dst) * elementSize, startElement * elementSize);
    }

    virtual void write(void* src, size_t size, size_t offset) = 0;

    template<std::ranges::contiguous_range T>
    void write(const T& src, size_t startElement = 0) {
        constexpr size_t elementSize = sizeof(std::ranges::range_value_t<T>);
        assert(elementSize == element_size());
        write((void*)std::data(src), std::size(src) * elementSize, startElement * elementSize);
    }

    DUK_NO_DISCARD virtual size_t size() const = 0;

    DUK_NO_DISCARD virtual size_t element_size() const = 0;

    DUK_NO_DISCARD virtual size_t byte_size() const = 0;

    DUK_NO_DISCARD virtual Type type() const = 0;

};

}

#endif // DUK_RENDERER_BUFFER_H

