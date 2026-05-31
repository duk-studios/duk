/// 04/08/2023
/// index_types.cpp

#include <duk_rhi/index_types.h>

#include <stdexcept>

namespace duk::rhi {

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
