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
    MeshMaterialResource material;
};

}

#endif // DUK_RENDERER_MESH_RENDERER_H

