//
// Created by Ricardo on 03/06/2024.
//

#ifndef DUK_RENDERER_DYNAMIC_MESH_H
#define DUK_RENDERER_DYNAMIC_MESH_H

#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>

namespace duk::renderer {

struct DynamicMeshCreateInfo {
    MeshBufferPool* meshBufferPool{};
    VertexAttributes attributes;
    duk::rhi::IndexType indexType{};
    uint32_t maxVertexCount{};
    uint32_t maxIndexCount{};
    duk::rhi::Buffer::UpdateFrequency updateFrequency{};
};

class DynamicMesh : public Mesh {
public:
    explicit DynamicMesh(const DynamicMeshCreateInfo& dynamicMeshCreateInfo);

    ~DynamicMesh() override;

    void write_vertices(VertexAttributes::Type attribute, const void* data, uint32_t size);

    template<std::ranges::contiguous_range TRange>
    void write_vertices(VertexAttributes::Type attribute, const TRange& range);

    void write_indices(const void* data, uint32_t size);

    template<std::ranges::contiguous_range TRange>
    void write_indices(const TRange& range);

    void flush();

    void clear();

    void draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) override;

private:
    VertexAttributes m_attributes;
    duk::rhi::IndexType m_indexType;
    uint32_t m_maxVertexCount;
    uint32_t m_maxIndexCount;
    MeshBuffer* m_meshBuffer;
    uint32_t m_meshBufferHandle;
    uint32_t m_firstVertex;
    uint32_t m_firstIndex;
    std::array<uint32_t, VertexAttributes::COUNT> m_vertexCounts;
    uint32_t m_indexCount;
};

template<std::ranges::contiguous_range TRange>
void DynamicMesh::write_vertices(VertexAttributes::Type attribute, const TRange& range) {
    using T = std::ranges::range_value_t<TRange>;
    DUK_ASSERT(VertexAttributes::size_of(attribute) == sizeof(T));
    write_vertices(attribute, std::data(range), std::size(range) * sizeof(T));
}

template<std::ranges::contiguous_range TRange>
void DynamicMesh::write_indices(const TRange& range) {
    using T = std::ranges::range_value_t<TRange>;
    DUK_ASSERT(duk::rhi::index_size(m_indexType) == sizeof(T));
    write_indices(std::data(range), std::size(range) * sizeof(T));
}

}// namespace duk::renderer

#endif//DUK_RENDERER_DYNAMIC_MESH_H
