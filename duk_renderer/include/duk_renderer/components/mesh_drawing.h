/// 20/08/2023
/// mesh_drawer.h

#ifndef DUK_RENDERER_MESH_DRAWER_H
#define DUK_RENDERER_MESH_DRAWER_H

namespace duk::renderer {

class Mesh;
class Material;

struct MeshDrawing {
    Mesh* mesh;
    Material* material;
};

}

#endif // DUK_RENDERER_MESH_DRAWER_H

