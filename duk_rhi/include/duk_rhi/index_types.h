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

bool is_index_buffer(Buffer::Type type);

IndexType index_type_from_buffer_type(Buffer::Type type);

Buffer::Type buffer_type_from_index_type(IndexType indexType);

size_t index_size(IndexType indexType);

template<typename T>
inline IndexType index_type() = delete;

template<>
inline IndexType index_type<uint16_t>() {
    return IndexType::UINT16;
}

template<>
inline IndexType index_type<uint32_t>() {
    return IndexType::UINT32;
}

}

#endif // DUK_RHI_INDEX_TYPES_H

