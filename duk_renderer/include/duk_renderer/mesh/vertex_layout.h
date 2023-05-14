//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_VERTEX_LAYOUT_H
#define DUK_RENDERER_VERTEX_LAYOUT_H

#include <duk_renderer/macros.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <array>

namespace duk::renderer {

class VertexAttribute {
public:
    enum class Format {
        UNDEFINED = -1,
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        FLOAT,
        VEC2,
        VEC3,
        VEC4
    };

    enum Type {
        UNDEFINED = -1,
        POSITION,
        COLOR,
        COUNT // number of types
    };

public:

    DUK_NO_DISCARD static size_t format_size(Format format);

    template<typename T>
    DUK_NO_DISCARD static Format format_of() = delete;
};

class VertexLayout {
public:

    VertexLayout();

    VertexLayout(std::initializer_list<std::pair<VertexAttribute::Type, VertexAttribute::Format>> attributes);

    void enable(VertexAttribute::Type type, VertexAttribute::Format format);

    void disable(VertexAttribute::Type type);

private:
    std::array<VertexAttribute::Format, VertexAttribute::Type::COUNT> m_attributes;
};

//UINT8,
template<>
VertexAttribute::Format VertexAttribute::format_of<uint8_t>() {
    return VertexAttribute::Format::UINT8;
}
//INT8,
template<>
VertexAttribute::Format VertexAttribute::format_of<int8_t>() {
    return VertexAttribute::Format::INT8;
}
//UINT16,
template<>
VertexAttribute::Format VertexAttribute::format_of<uint16_t>() {
    return VertexAttribute::Format::UINT16;
}
//INT16,
template<>
VertexAttribute::Format VertexAttribute::format_of<int16_t>() {
    return VertexAttribute::Format::INT16;
}
//UINT32,
template<>
VertexAttribute::Format VertexAttribute::format_of<uint32_t>() {
    return VertexAttribute::Format::UINT32;
}
//INT32,
template<>
VertexAttribute::Format VertexAttribute::format_of<int32_t>() {
    return VertexAttribute::Format::INT32;
}
//FLOAT,
template<>
VertexAttribute::Format VertexAttribute::format_of<float>() {
    return VertexAttribute::Format::FLOAT;
}
//VEC2,
template<>
VertexAttribute::Format VertexAttribute::format_of<glm::vec2>() {
    return VertexAttribute::Format::VEC2;
}
//VEC3,
template<>
VertexAttribute::Format VertexAttribute::format_of<glm::vec3>() {
    return VertexAttribute::Format::VEC3;
}
//VEC4
template<>
VertexAttribute::Format VertexAttribute::format_of<glm::vec4>() {
    return VertexAttribute::Format::VEC4;
}

}

#endif //DUK_RENDERER_VERTEX_LAYOUT_H
