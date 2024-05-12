//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_LAYOUT_H
#define DUK_RHI_VERTEX_LAYOUT_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <vector>
#include <span>

namespace duk::rhi {

class VertexInput {
public:
    enum class Format : uint32_t {
        UNDEFINED = 0,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        FLOAT32,
        VEC2,
        VEC3,
        VEC4
    };

    static size_t size_of(Format type);

    template<typename T>
    DUK_NO_DISCARD static Format format_of() = delete;
};

class VertexLayout {
public:
    using Container = std::vector<VertexInput::Format>;

    VertexLayout();

    VertexLayout(const std::initializer_list<VertexInput::Format>& formats);

    void clear();

    void insert(VertexInput::Format format);

    void insert(const std::initializer_list<VertexInput::Format>& formats);

    void insert(const std::span<VertexInput::Format>& formats);

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD size_t byte_size() const;

    DUK_NO_DISCARD Container::iterator begin();

    DUK_NO_DISCARD Container::iterator end();

    DUK_NO_DISCARD Container::const_iterator begin() const;

    DUK_NO_DISCARD Container::const_iterator end() const;

    DUK_NO_DISCARD VertexInput::Format format_at(uint32_t location) const;

private:
    std::vector<VertexInput::Format> m_formats;
};

//UINT8,
template<>
inline VertexInput::Format VertexInput::format_of<uint8_t>() {
    return VertexInput::Format::UINT8;
}

//INT8,
template<>
inline VertexInput::Format VertexInput::format_of<int8_t>() {
    return VertexInput::Format::INT8;
}

//UINT16,
template<>
inline VertexInput::Format VertexInput::format_of<uint16_t>() {
    return VertexInput::Format::UINT16;
}

//INT16,
template<>
inline VertexInput::Format VertexInput::format_of<int16_t>() {
    return VertexInput::Format::INT16;
}

//UINT32,
template<>
inline VertexInput::Format VertexInput::format_of<uint32_t>() {
    return VertexInput::Format::UINT32;
}

//INT32,
template<>
inline VertexInput::Format VertexInput::format_of<int32_t>() {
    return VertexInput::Format::INT32;
}

//FLOAT,
template<>
inline VertexInput::Format VertexInput::format_of<float>() {
    return VertexInput::Format::FLOAT32;
}

//VEC2,
template<>
inline VertexInput::Format VertexInput::format_of<glm::vec2>() {
    return VertexInput::Format::VEC2;
}

//VEC3,
template<>
inline VertexInput::Format VertexInput::format_of<glm::vec3>() {
    return VertexInput::Format::VEC3;
}

//VEC4
template<>
inline VertexInput::Format VertexInput::format_of<glm::vec4>() {
    return VertexInput::Format::VEC4;
}

}// namespace duk::rhi

namespace std {
template<>
struct hash<duk::rhi::VertexLayout> {
    size_t operator()(const duk::rhi::VertexLayout& vertexLayout) {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertexLayout.begin(), vertexLayout.end());
        return hash;
    }
};
}// namespace std
#endif//DUK_RHI_VERTEX_LAYOUT_H
