/// 04/08/2023
/// mesh_builtins.cpp

#include <duk_renderer/mesh/static_mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>

namespace duk::renderer {

StaticMesh::StaticMesh(const StaticMeshCreateInfo& staticMeshCreateInfo)
    : m_meshBufferPool(staticMeshCreateInfo.meshBufferPool)
    , m_meshBuffer(nullptr)
    , m_meshBufferHandle(0)
    , m_firstVertex(~0)
    , m_vertexCount(~0)
    , m_firstIndex(~0)
    , m_indexCount(~0)
    , m_indexType(rhi::IndexType::NONE) {
    auto meshDataSource = staticMeshCreateInfo.meshDataSource;
    auto vertexAttributes = meshDataSource->vertex_attributes();
    m_vertexLayout = vertexAttributes.vertex_layout();
    m_indexType = meshDataSource->index_type();
    m_vertexCount = meshDataSource->vertex_count();
    m_indexCount = meshDataSource->index_count();

    m_meshBuffer = m_meshBufferPool->find_buffer(m_vertexLayout, m_indexType, duk::rhi::Buffer::UpdateFrequency::STATIC);

    m_meshBufferHandle = m_meshBuffer->allocate(m_vertexCount, m_indexCount);

    for (auto attributeIndex: meshDataSource->vertex_attributes()) {
        auto attribute = static_cast<VertexAttributes::Type>(attributeIndex);
        std::vector<uint8_t> buffer(m_vertexCount * VertexAttributes::size_of(attribute));
        meshDataSource->read_vertices_attribute(attribute, buffer.data(), m_vertexCount, 0);
        m_meshBuffer->write_vertex(m_meshBufferHandle, attributeIndex, buffer.data(), buffer.size(), 0);
    }

    if (m_indexType != duk::rhi::IndexType::NONE) {
        std::vector<uint8_t> buffer(m_indexCount * duk::rhi::index_size(m_indexType));
        meshDataSource->read_indices(buffer.data(), m_indexCount, 0);
        m_meshBuffer->write_index(m_meshBufferHandle, buffer.data(), buffer.size(), 0);
    }

    m_meshBuffer->flush();

    m_firstVertex = m_meshBuffer->first_vertex(m_meshBufferHandle);
    m_firstIndex = m_meshBuffer->first_index(m_meshBufferHandle);
}

StaticMesh::~StaticMesh() {
    m_meshBuffer->free(m_meshBufferHandle);
}

rhi::VertexLayout StaticMesh::vertex_layout() const {
    return m_vertexLayout;
}

rhi::IndexType StaticMesh::index_type() const {
    return m_indexType;
}

const MeshBuffer* StaticMesh::buffer() const {
    return m_meshBuffer;
}

uint32_t StaticMesh::vertex_count() const {
    return m_vertexCount;
}

uint32_t StaticMesh::vertex_offset() const {
    return m_firstVertex;
}

uint32_t StaticMesh::index_count() const {
    return m_indexCount;
}

uint32_t StaticMesh::index_offset() const {
    return m_firstIndex;
}

}// namespace duk::renderer