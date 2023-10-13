//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_VERTEX_TYPES_H
#define DUK_RHI_VERTEX_TYPES_H

#include <duk_rhi/vertex_layout.h>

namespace duk::renderer {

/// specialize this template method for every vertex type
template<typename T>
rhi::VertexLayout layout_of() = delete;

struct Vertex2DColor {
    glm::vec2 position;
    glm::vec4 color;
};

template<>
inline rhi::VertexLayout layout_of<Vertex2DColor>() {
    return {
        rhi::VertexAttribute::format_of<glm::vec2>(),
        rhi::VertexAttribute::format_of<glm::vec4>()
    };
}

struct Vertex2DColorUV {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
inline rhi::VertexLayout layout_of<Vertex2DColorUV>() {
    return {
        rhi::VertexAttribute::format_of<glm::vec2>(),
        rhi::VertexAttribute::format_of<glm::vec4>(),
        rhi::VertexAttribute::format_of<glm::vec2>()
    };
}

struct Vertex3DColorUV {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

template<>
inline rhi::VertexLayout layout_of<Vertex3DColorUV>() {
    return {
            rhi::VertexAttribute::format_of<glm::vec3>(),
            rhi::VertexAttribute::format_of<glm::vec4>(),
            rhi::VertexAttribute::format_of<glm::vec2>()
    };
}

struct Vertex3DNormalUV {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

template<>
inline rhi::VertexLayout layout_of<Vertex3DNormalUV>() {
    return {
            rhi::VertexAttribute::format_of<glm::vec3>(),
            rhi::VertexAttribute::format_of<glm::vec3>(),
            rhi::VertexAttribute::format_of<glm::vec2>()
    };
}

}

// specialize std::hash for every vertex type
namespace std {
template<>
struct hash<duk::renderer::Vertex2DColor> {
    size_t operator()(const duk::renderer::Vertex2DColor& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        return hash;
    }
};

template<>
struct hash<duk::renderer::Vertex2DColorUV> {
    size_t operator()(const duk::renderer::Vertex2DColorUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

template<>
struct hash<duk::renderer::Vertex3DColorUV> {
    size_t operator()(const duk::renderer::Vertex3DColorUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.color);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

template<>
struct hash<duk::renderer::Vertex3DNormalUV> {
    size_t operator()(const duk::renderer::Vertex3DNormalUV& vertex){
        size_t hash = 0;
        duk::hash::hash_combine(hash, vertex.position);
        duk::hash::hash_combine(hash, vertex.normal);
        duk::hash::hash_combine(hash, vertex.uv);
        return hash;
    }
};

}

#endif //DUK_RHI_VERTEX_TYPES_H
