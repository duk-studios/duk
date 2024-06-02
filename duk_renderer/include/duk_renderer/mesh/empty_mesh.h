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

    uint32_t vertex_count() const;

    void draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) override;

private:
    uint32_t m_vertexCount;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_EMPTY_MESH_H
