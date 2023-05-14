//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_HASH_COMMON_HASHES_H
#define DUK_HASH_COMMON_HASHES_H

#include <duk_hash/hash_combine.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace std {

template<>
struct hash<glm::vec2> {
    size_t operator()(const glm::vec2& vec) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vec.x);
        duk::hash::hash_combine(hash, vec.y);
        return hash;
    }
};

template<>
struct hash<glm::vec3> {
    size_t operator()(const glm::vec3& vec) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vec.x);
        duk::hash::hash_combine(hash, vec.y);
        duk::hash::hash_combine(hash, vec.z);
        return hash;
    }
};

template<>
struct hash<glm::vec4> {
    size_t operator()(const glm::vec4& vec) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vec.x);
        duk::hash::hash_combine(hash, vec.y);
        duk::hash::hash_combine(hash, vec.z);
        duk::hash::hash_combine(hash, vec.w);
        return hash;
    }
};

}

#endif //DUK_HASH_COMMON_HASHES_H
