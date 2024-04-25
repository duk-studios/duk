/// 04/08/2023
/// mesh_pool.h

#ifndef DUK_RENDERER_MESH_H
#define DUK_RENDERER_MESH_H

#include <duk_renderer/brush.h>
#include <duk_renderer/mesh/mesh_data_source.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

#include <unordered_map>

namespace duk::renderer {

class MeshBuffer;
class MeshBufferPool;

struct MeshCreateInfo {
    MeshBufferPool* meshBufferPool;
    const MeshDataSource* meshDataSource;
};

class Mesh : public Brush {
public:
    explicit Mesh(const MeshCreateInfo& meshCreateInfo);

    ~Mesh() override;

    DUK_NO_DISCARD rhi::VertexLayout vertex_layout() const;

    DUK_NO_DISCARD rhi::IndexType index_type() const;

    void draw(rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) override;

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

#endif// DUK_RENDERER_MESH_H
