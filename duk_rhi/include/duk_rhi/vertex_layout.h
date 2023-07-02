//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_VERTEX_LAYOUT_H
#define DUK_RHI_VERTEX_LAYOUT_H

#include <duk_macros/macros.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <vector>

namespace duk::rhi {

class VertexAttribute {
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

    using Container = std::vector<VertexAttribute::Format>;

    VertexLayout();

    VertexLayout(const std::initializer_list<VertexAttribute::Format>& attributes);

    void insert(VertexAttribute::Format attributes);

    void insert(const std::initializer_list<VertexAttribute::Format>& attributes);

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD size_t byte_size() const;

    DUK_NO_DISCARD Container::iterator begin();

    DUK_NO_DISCARD Container::iterator end();

    DUK_NO_DISCARD Container::const_iterator begin() const;

    DUK_NO_DISCARD Container::const_iterator end() const;

    DUK_NO_DISCARD VertexAttribute::Format format_at(uint32_t location) const;

private:
    std::vector<VertexAttribute::Format> m_attributes;
};

//UINT8,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<uint8_t>() {
    return VertexAttribute::Format::UINT8;
}
//INT8,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<int8_t>() {
    return VertexAttribute::Format::INT8;
}
//UINT16,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<uint16_t>() {
    return VertexAttribute::Format::UINT16;
}
//INT16,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<int16_t>() {
    return VertexAttribute::Format::INT16;
}
//UINT32,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<uint32_t>() {
    return VertexAttribute::Format::UINT32;
}
//INT32,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<int32_t>() {
    return VertexAttribute::Format::INT32;
}
//FLOAT,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<float>() {
    return VertexAttribute::Format::FLOAT32;
}
//VEC2,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<glm::vec2>() {
    return VertexAttribute::Format::VEC2;
}
//VEC3,
template<>
inline VertexAttribute::Format VertexAttribute::format_of<glm::vec3>() {
    return VertexAttribute::Format::VEC3;
}
//VEC4
template<>
inline VertexAttribute::Format VertexAttribute::format_of<glm::vec4>() {
    return VertexAttribute::Format::VEC4;
}

}

#endif //DUK_RHI_VERTEX_LAYOUT_H
