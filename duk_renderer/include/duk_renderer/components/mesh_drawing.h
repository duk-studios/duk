/// 20/08/2023
/// mesh_drawer.h

#ifndef DUK_RENDERER_MESH_DRAWER_H
#define DUK_RENDERER_MESH_DRAWER_H

namespace duk::renderer {

class Mesh;
class Painter;
class Material;

struct MeshDrawing {
    Mesh* mesh;
    Painter* painter;
    Material* material;
};

}

#endif // DUK_RENDERER_MESH_DRAWER_H

