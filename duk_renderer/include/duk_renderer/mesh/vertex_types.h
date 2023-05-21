//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RENDERER_VERTEX_TYPES_H
#define DUK_RENDERER_VERTEX_TYPES_H

#include <duk_renderer/mesh/vertex_layout.h>

namespace duk::renderer {

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
}

#endif //DUK_RENDERER_VERTEX_TYPES_H
