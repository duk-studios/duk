/// 04/08/2023
/// index_types.h

#ifndef DUK_RHI_INDEX_TYPES_H
#define DUK_RHI_INDEX_TYPES_H

#include <duk_rhi/buffer.h>

namespace duk::rhi {

enum class IndexType {
    NONE,
    UINT16,
    UINT32
};

size_t index_size(IndexType indexType);

template<typename T>
IndexType index_type() = delete;

template<>
inline IndexType index_type<uint16_t>() {
    return IndexType::UINT16;
}

template<>
inline IndexType index_type<uint32_t>() {
    return IndexType::UINT32;
}

}// namespace duk::rhi

#endif// DUK_RHI_INDEX_TYPES_H
