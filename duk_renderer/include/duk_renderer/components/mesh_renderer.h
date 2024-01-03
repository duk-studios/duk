/// 20/08/2023
/// mesh_renderer.h

#ifndef DUK_RENDERER_MESH_RENDERER_H
#define DUK_RENDERER_MESH_RENDERER_H

#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/material_pool.h>

namespace duk::renderer {

class Mesh;
class Material;

struct MeshRenderer {
    MeshResource mesh;
    MaterialResource material;
};

template<>
inline SortKey SortKey::calculate<MeshRenderer>(const MeshRenderer& param) {
    SortKey::Flags flags = {};
    flags.materialValue = reinterpret_cast<std::intptr_t>(param.material.get());
    flags.meshValue = reinterpret_cast<std::intptr_t>(param.mesh.get());
    return SortKey{flags};
}

}

#endif // DUK_RENDERER_MESH_RENDERER_H

