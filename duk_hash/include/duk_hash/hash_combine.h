//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_HASH_HASH_COMBINE_H
#define DUK_HASH_HASH_COMBINE_H

#include <functional>
#include <cstdint>

namespace duk::hash {

using Hash = uint64_t;

constexpr Hash UndefinedHash = Hash(-1);

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
    std::hash<T> hash_f;
    for (auto i = 0; i < n; i++) {
        hash_combine(seed, *(begin + i));
    }
}

} // duk::hash

#endif //DUK_HASH_HASH_COMBINE_H
