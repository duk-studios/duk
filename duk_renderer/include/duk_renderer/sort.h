/// 30/09/2023
/// sort.h

#ifndef DUK_RENDERER_SORT_H
#define DUK_RENDERER_SORT_H

#include <duk_objects/objects.h>

#include <cstdint>
#include <numeric>

namespace duk::renderer {

struct SortKey {
    struct Bytes8x8 {
        std::array<uint8_t, 8> chunks;
    };

    struct Bytes16x4 {
        std::array<uint16_t, 4> chunks;
    };

    struct Bytes32x2 {
        std::array<uint32_t, 2> chunks;
    };

    struct Bytes64x1 {
        std::array<uint64_t, 1> chunks;
    };

    union {
        Bytes8x8 bytes8x8;
        Bytes16x4 bytes16x4;
        Bytes32x2 bytes32x2;
        Bytes64x1 bytes64x1;
        uint64_t key;
    };

    // specialize this method for any given type to access its SortKey object
    template<typename T>
    static SortKey sort_key(const T&) = delete;

    // receives an array of objects that have sort keys, and populate another indices array sorted by the first array sort keys
    // ObjectEntriesContainerT may be a static array, but ObjectIndicesContainerT
    // has to be a dynamic one (at least with the resize member function)
    template<typename ObjectEntriesContainerT, typename ObjectIndicesContainerT>
    static void sort_indices(const ObjectEntriesContainerT& objectEntries, ObjectIndicesContainerT& sortedIndices);
};

template<typename ObjectEntriesContainerT, typename ObjectIndicesContainerT>
void SortKey::sort_indices(const ObjectEntriesContainerT& objectEntries, ObjectIndicesContainerT& sortedIndices) {
    sortedIndices.resize(std::size(objectEntries));

    // fill vector with 0 ... objectsEntries.size()
    std::iota(std::begin(sortedIndices), std::end(sortedIndices), 0);

    // sort indices based on SortKey
    std::sort(std::begin(sortedIndices), std::end(sortedIndices), [&objectEntries](const auto& lhs, const auto& rhs) {
        return sort_key(objectEntries[lhs]).key < sort_key(objectEntries[rhs]).key;
    });
}

}// namespace duk::renderer

#endif// DUK_RENDERER_SORT_H
