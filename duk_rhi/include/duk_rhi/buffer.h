/// 28/05/2023
/// buffer.h

#ifndef DUK_RHI_BUFFER_H
#define DUK_RHI_BUFFER_H

#include <duk_macros/macros.h>

#include <cstdint>

namespace duk::rhi {

class Buffer {
public:
    enum class UpdateFrequency {
        STATIC,  ///< Device-local; written via staging through the command context.
        DYNAMIC  ///< Host-visible; written directly by the CPU through the command context.
    };

    enum class Type {
        INDEX_16,
        INDEX_32,
        VERTEX,
        UNIFORM,
        STORAGE,
        INDIRECT
    };

    virtual ~Buffer() = default;

    DUK_NO_DISCARD virtual size_t size() const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_BUFFER_H
