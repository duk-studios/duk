/// 11/10/2023
/// empty_mesh.h

#ifndef DUK_RENDERER_EMPTY_MESH_H
#define DUK_RENDERER_EMPTY_MESH_H

#include <duk_renderer/mesh/mesh.h>

namespace duk::renderer {

struct EmptyMeshCreateInfo {
    uint32_t vertexCount;
};

class EmptyMesh : public Mesh {
public:
    explicit EmptyMesh(const EmptyMeshCreateInfo& emptyMeshCreateInfo);

    const MeshBuffer* buffer() const override;

    uint32_t vertex_count() const override;

    uint32_t vertex_offset() const override;

    uint32_t index_count() const override;

    uint32_t index_offset() const override;

private:
    uint32_t m_vertexCount;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_EMPTY_MESH_H
