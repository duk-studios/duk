/// 04/08/2023
/// mesh_pool.h

#ifndef DUK_RENDERER_STATIC_MESH_H
#define DUK_RENDERER_STATIC_MESH_H

#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/mesh/static_mesh_data_source.h>

namespace duk::renderer {

class MeshBuffer;
class MeshBufferPool;

struct StaticMeshCreateInfo {
    MeshBufferPool* meshBufferPool;
    const StaticMeshDataSource* meshDataSource;
};

class StaticMesh : public Mesh {
public:
    explicit StaticMesh(const StaticMeshCreateInfo& staticMeshCreateInfo);

    ~StaticMesh() override;

    DUK_NO_DISCARD rhi::VertexLayout vertex_layout() const;

    DUK_NO_DISCARD rhi::IndexType index_type() const;

    const MeshBuffer* buffer() const override;

    uint32_t vertex_count() const override;

    uint32_t vertex_offset() const override;

    uint32_t index_count() const override;

    uint32_t index_offset() const override;

private:
    MeshBufferPool* m_meshBufferPool;
    MeshBuffer* m_meshBuffer;
    uint32_t m_meshBufferHandle;
    duk::rhi::VertexLayout m_vertexLayout;
    duk::rhi::IndexType m_indexType;
    uint32_t m_firstVertex;
    uint32_t m_vertexCount;
    uint32_t m_firstIndex;
    uint32_t m_indexCount;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_STATIC_MESH_H
