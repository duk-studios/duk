/// 04/08/2023
/// index_types.cpp

#include <duk_rhi/index_types.h>

#include <stdexcept>

namespace duk::rhi {

bool is_index_buffer(Buffer::Type type) {
    return type == Buffer::Type::INDEX_16 || type == Buffer::Type::INDEX_32;
}

IndexType index_type_from_buffer_type(Buffer::Type type) {
    switch (type) {
        case Buffer::Type::INDEX_16:
            return IndexType::UINT16;
        case Buffer::Type::INDEX_32:
            return IndexType::UINT32;
        default:
            throw std::invalid_argument("invalid buffer type for index buffer");
    }
}

Buffer::Type buffer_type_from_index_type(IndexType indexType) {
    switch (indexType) {
        case IndexType::UINT16:
            return Buffer::Type::INDEX_16;
        case IndexType::UINT32:
            return Buffer::Type::INDEX_32;
        default:
            throw std::invalid_argument("undefined buffer type for IndexType");
    }
}

size_t index_size(IndexType indexType) {
    switch (indexType) {
        case IndexType::NONE:
            return 0;
        case IndexType::UINT16:
            return 2;
        case IndexType::UINT32:
            return 4;
        default:
            throw std::invalid_argument("undefined index size for IndexType");
    }
}

}// namespace duk::rhi
