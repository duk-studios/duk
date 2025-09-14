//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_HASH_HASH_COMBINE_H
#define DUK_HASH_HASH_COMBINE_H

#include <cstdint>
#include <functional>

namespace duk::hash {

using Hash = std::size_t;

constexpr Hash kUndefinedHash = Hash(~0llu);

//from boost
template<typename T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename InputIterator>
inline void hash_combine(std::size_t& seed, InputIterator begin, InputIterator end) {
    using T = typename std::iterator_traits<InputIterator>::value_type;

    auto n = std::distance(begin, end);

    /// iterates over all entries combining their hashes
    for (auto i = 0; i < n; i++) {
        hash_combine(seed, *(begin + i));
    }
}

static inline void hash_combine(std::size_t& seed, const void* data, size_t size) {
    auto uint64Ptr = (const uint64_t*)data;
    auto uint64PtrEnd = uint64Ptr + (size / sizeof(uint64_t));

    // write 8 byte blocks
    while (uint64Ptr < uint64PtrEnd) {
        hash_combine(seed, *uint64Ptr);
        uint64Ptr++;
    }

    // write remaining bytes
    auto uint8Ptr = (const uint8_t*)uint64Ptr;
    auto uint8PtrEnd = (const uint8_t*)data + size;
    while (uint8Ptr < uint8PtrEnd) {
        hash_combine(seed, *uint8Ptr);
        uint8Ptr++;
    }
}

}// namespace duk::hash

#endif//DUK_HASH_HASH_COMBINE_H
