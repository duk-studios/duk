/// 30/09/2023
/// sort.h

#ifndef DUK_RENDERER_SORT_H
#define DUK_RENDERER_SORT_H

#include <duk_scene/scene.h>

#include <cstdint>
#include <numeric>

namespace duk::renderer {

struct SortKey {
    struct Flags {
        uint16_t meshValue;
        uint16_t materialValue;
    };
    union {
        Flags flags;
        uint32_t key;
    };

    template<typename T>
    static SortKey calculate(const T& param) = delete;

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

}

#endif // DUK_RENDERER_SORT_H

