/// 20/08/2023
/// mesh_drawer.h

#ifndef DUK_RENDERER_MESH_DRAWER_H
#define DUK_RENDERER_MESH_DRAWER_H

#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/material_pool.h>

namespace duk::renderer {

class Mesh;
class Material;

struct MeshDrawing {
    MeshResource mesh;
    MaterialResource material;
};

template<>
inline SortKey SortKey::calculate<MeshDrawing>(const MeshDrawing& param) {
    SortKey::Flags flags = {};
    flags.materialValue = reinterpret_cast<std::intptr_t>(param.material.get());
    flags.meshValue = reinterpret_cast<std::intptr_t>(param.mesh.get());
    return SortKey{flags};
}

}

#endif // DUK_RENDERER_MESH_DRAWER_H

