//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RENDERER_VERTEX_TYPES_H
#define DUK_RENDERER_VERTEX_TYPES_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/vertex_layout.h>
#include <duk_tools/bit_block.h>

#include <memory>
#include <span>
#include <set>

namespace duk::renderer {

class VertexAttributes {
public:
    enum Type {
        POSITION = 0,
        NORMAL,
        UV,
        COLOR,
        COUNT
    };

    using Mask = duk::tools::BitBlock<COUNT>;
    using ConstIterator = Mask::BitBlockIterator<true>;

    template<typename T>
    static VertexAttributes create() = delete;

    static duk::rhi::VertexInput::Format format_of(Type attributeType);

    static size_t size_of(Type attributeType);

    VertexAttributes();

    explicit VertexAttributes(const std::set<Type>& attributes);

    DUK_NO_DISCARD bool has_attribute(Type attributeType) const;

    DUK_NO_DISCARD const rhi::VertexLayout& vertex_layout() const;

    DUK_NO_DISCARD size_t offset_of(Type attributeType) const;

    DUK_NO_DISCARD ConstIterator begin() const;

    DUK_NO_DISCARD ConstIterator end() const;

private:
    Mask m_attributes;
    std::array<size_t, COUNT> m_attributeOffset;
    duk::rhi::VertexLayout m_layout;
};

using VertexBufferArray = std::array<std::shared_ptr<duk::rhi::Buffer>, VertexAttributes::COUNT>;
using VertexBufferRefArray = std::array<duk::rhi::Buffer*, VertexAttributes::COUNT>;

// ---------------------------

struct VertexColorUV {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
inline VertexAttributes VertexAttributes::create<VertexColorUV>() {
    return VertexAttributes({POSITION, COLOR, UV});
}

// ---------------------------

struct VertexNormalUV {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

template<>
inline VertexAttributes VertexAttributes::create<VertexNormalUV>() {
    return VertexAttributes({POSITION, NORMAL, UV});
}

}// namespace duk::renderer

// specialize std::hash for every vertex type
namespace std {

// ---------------------------

template<>
struct hash<duk::renderer::VertexColorUV> {
    size_t operator()(const duk::renderer::VertexColorUV& vertex) const {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

// ---------------------------

template<>
struct hash<duk::renderer::VertexNormalUV> {
    size_t operator()(const duk::renderer::VertexNormalUV& vertex) const {
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.normal);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

}// namespace std

#endif//DUK_RENDERER_VERTEX_TYPES_H
