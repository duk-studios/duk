//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_VERTEX_TYPES_H
#define DUK_RHI_VERTEX_TYPES_H

#include <duk_rhi/vertex_layout.h>
#include <duk_tools/bit_block.h>
#include <bitset>

namespace duk::renderer {

class VertexAttributes {
public:

    enum class Type {
        POSITION = 0,
        NORMAL,
        UV,
        COLOR,
        COUNT
    };

    using ConstIterator = std::vector<Type>::const_iterator;

    template<typename T>
    static VertexAttributes create() = delete;

    static duk::rhi::VertexInput::Format format_of(Type attributeType);

    VertexAttributes();

    VertexAttributes(std::vector<VertexAttributes::Type>&& attributes);

    bool has_attribute(Type attributeType) const;

    const rhi::VertexLayout& vertex_layout() const;

    size_t offset_of(Type attributeType) const;

    ConstIterator begin() const;

    ConstIterator end() const;

private:
    std::vector<VertexAttributes::Type> m_attributes;
    duk::rhi::VertexLayout m_layout;
};

// ---------------------------

struct VertexColorUV {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
VertexAttributes VertexAttributes::create<VertexColorUV>() {
    return VertexAttributes({VertexAttributes::Type::POSITION, VertexAttributes::Type::COLOR, VertexAttributes::Type::UV});
}

// ---------------------------

struct VertexNormalUV {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

template<>
VertexAttributes VertexAttributes::create<VertexNormalUV>() {
    return VertexAttributes({VertexAttributes::Type::POSITION, VertexAttributes::Type::NORMAL, VertexAttributes::Type::UV});
}

}

// specialize std::hash for every vertex type
namespace std {

// ---------------------------

template<>
struct hash<duk::renderer::VertexColorUV> {
    size_t operator()(const duk::renderer::VertexColorUV& vertex){
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
    size_t operator()(const duk::renderer::VertexNormalUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.normal);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

}

#endif //DUK_RHI_VERTEX_TYPES_H
