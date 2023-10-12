/// 20/08/2023
/// mesh_drawer.h

#ifndef DUK_RENDERER_MESH_DRAWER_H
#define DUK_RENDERER_MESH_DRAWER_H

#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/painters/painter.h>
#include <duk_renderer/painters/palette.h>

namespace duk::renderer {

struct MeshDrawing {
    Mesh* mesh;
    Painter* painter;
    Palette* palette;
};

}

#endif // DUK_RENDERER_MESH_DRAWER_H

