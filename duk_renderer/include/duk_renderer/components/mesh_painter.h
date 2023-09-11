/// 20/08/2023
/// mesh_drawer.h

#ifndef DUK_RENDERER_MESH_DRAWER_H
#define DUK_RENDERER_MESH_DRAWER_H

#include <duk_renderer/mesh.h>
#include <duk_renderer/painters/painter.h>

namespace duk::renderer {

struct MeshPainter {
    Mesh* mesh;
    Painter* painter;
    duk::rhi::DescriptorSet* instanceDescriptorSet;
};

}

#endif // DUK_RENDERER_MESH_DRAWER_H

