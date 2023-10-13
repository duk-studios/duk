//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_VERTEX_TYPES_H
#define DUK_RHI_VERTEX_TYPES_H

#include <duk_rhi/vertex_layout.h>

namespace duk::rhi {

/// specialize this template method for every vertex type
template<typename T>
VertexLayout layout_of() = delete;

struct Vertex2DColor {
    glm::vec2 position;
    glm::vec4 color;
};

template<>
inline VertexLayout layout_of<Vertex2DColor>() {
    return {
        VertexAttribute::format_of<glm::vec2>(),
        VertexAttribute::format_of<glm::vec4>()
    };
}

struct Vertex2DColorUV {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
inline VertexLayout layout_of<Vertex2DColorUV>() {
    return {
        VertexAttribute::format_of<glm::vec2>(),
        VertexAttribute::format_of<glm::vec4>(),
        VertexAttribute::format_of<glm::vec2>()
    };
}

struct Vertex3DColorUV {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
inline VertexLayout layout_of<Vertex3DColorUV>() {
    return {
            VertexAttribute::format_of<glm::vec3>(),
            VertexAttribute::format_of<glm::vec4>(),
            VertexAttribute::format_of<glm::vec2>()
    };
}

struct Vertex3DNormalUV {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

template<>
inline VertexLayout layout_of<Vertex3DNormalUV>() {
    return {
            VertexAttribute::format_of<glm::vec3>(),
            VertexAttribute::format_of<glm::vec3>(),
            VertexAttribute::format_of<glm::vec2>()
    };
}

}

// specialize std::hash for every vertex type
namespace std {
template<>
struct hash<duk::rhi::Vertex2DColor> {
    size_t operator()(const duk::rhi::Vertex2DColor& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        return hash;
    }
};

template<>
struct hash<duk::rhi::Vertex2DColorUV> {
    size_t operator()(const duk::rhi::Vertex2DColorUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

template<>
struct hash<duk::rhi::Vertex3DColorUV> {
    size_t operator()(const duk::rhi::Vertex3DColorUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

template<>
struct hash<duk::rhi::Vertex3DNormalUV> {
    size_t operator()(const duk::rhi::Vertex3DNormalUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.normal);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

}

#endif //DUK_RHI_VERTEX_TYPES_H
