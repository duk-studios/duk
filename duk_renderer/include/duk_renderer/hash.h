//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_HASH_H
#define DUK_RENDERER_HASH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <functional>
#include <cstdint>

namespace duk::renderer {

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

} // duk::renderer

/// common type hashes

namespace std {

template<>
struct hash<glm::vec2> {
    size_t operator()(const glm::vec2& vec) noexcept {
        size_t hash = 0;
        duk::renderer::hash_combine(hash, vec.x);
        duk::renderer::hash_combine(hash, vec.y);
        return hash;
    }
};

template<>
struct hash<glm::vec3> {
    size_t operator()(const glm::vec3& vec) noexcept {
        size_t hash = 0;
        duk::renderer::hash_combine(hash, vec.x);
        duk::renderer::hash_combine(hash, vec.y);
        duk::renderer::hash_combine(hash, vec.z);
        return hash;
    }
};

template<>
struct hash<glm::vec4> {
    size_t operator()(const glm::vec4& vec) noexcept {
        size_t hash = 0;
        duk::renderer::hash_combine(hash, vec.x);
        duk::renderer::hash_combine(hash, vec.y);
        duk::renderer::hash_combine(hash, vec.z);
        duk::renderer::hash_combine(hash, vec.w);
        return hash;
    }
};

}


#endif //DUK_RENDERER_HASH_H
