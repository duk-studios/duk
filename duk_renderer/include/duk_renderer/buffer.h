/// 28/05/2023
/// buffer.h

#ifndef DUK_RENDERER_BUFFER_H
#define DUK_RENDERER_BUFFER_H

#include <duk_macros/macros.h>

#include <cstdint>

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

    template<typename T>
    void read(T& dst, size_t index) {
        read(&dst, sizeof(T), index * sizeof(T));
    }

    virtual void write(void* src, size_t size, size_t offset) = 0;

    template<typename T>
    void write(const T& src, size_t index) {
        write(&src, sizeof(T), index * sizeof(T));
    }

    DUK_NO_DISCARD virtual size_t size() const = 0;

    DUK_NO_DISCARD virtual Type type() const = 0;

};

}

#endif // DUK_RENDERER_BUFFER_H

