//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_HASH_H
#define DUK_RENDERER_HASH_H

#include <functional>
#include <cstdint>

namespace duk::renderer {

using Hash = uint64_t;

constexpr Hash UndefinedHash = Hash(-1);

//from boost
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

}

#endif //DUK_RENDERER_HASH_H
