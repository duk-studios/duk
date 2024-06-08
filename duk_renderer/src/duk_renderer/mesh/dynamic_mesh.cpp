//
// Created by Ricardo on 03/06/2024.
//

#include <duk_renderer/mesh/dynamic_mesh.h>

namespace duk::renderer {

DynamicMesh::DynamicMesh(const DynamicMeshCreateInfo& dynamicMeshCreateInfo)
    : m_attributes(dynamicMeshCreateInfo.attributes)
    , m_indexType(dynamicMeshCreateInfo.indexType)
    , m_maxVertexCount(dynamicMeshCreateInfo.maxVertexCount)
    , m_maxIndexCount(dynamicMeshCreateInfo.maxIndexCount)
    , m_meshBuffer(nullptr)
    , m_firstVertex(0)
    , m_firstIndex(0)
    , m_vertexCounts({})
    , m_indexCount(0) {
    m_meshBuffer = dynamicMeshCreateInfo.meshBufferPool->find_buffer(m_attributes.vertex_layout(), m_indexType, dynamicMeshCreateInfo.updateFrequency);
    m_meshBufferHandle = m_meshBuffer->allocate(m_maxVertexCount, m_maxIndexCount);
    m_firstVertex = m_meshBuffer->first_vertex(m_meshBufferHandle);
    m_firstIndex = m_meshBuffer->first_index(m_meshBufferHandle);
}

DynamicMesh::~DynamicMesh() {
    m_meshBuffer->free(m_meshBufferHandle);
}

void DynamicMesh::write_vertices(duk::renderer::VertexAttributes::Type attribute, const void* data, uint32_t size) {
    DUK_ASSERT(m_attributes.has_attribute(attribute));
    auto& vertexCount = m_vertexCounts[attribute];
    const auto vertexSize = duk::renderer::VertexAttributes::size_of(attribute);
    const auto offset = vertexSize * vertexCount;
    m_meshBuffer->write_vertex(m_meshBufferHandle, attribute, data, size, offset);
    vertexCount += size / vertexSize;
}

void DynamicMesh::write_indices(const void* data, uint32_t size) {
    DUK_ASSERT(m_indexType != duk::rhi::IndexType::NONE);
    const auto indexSize = duk::rhi::index_size(m_indexType);
    const auto offset = indexSize * m_indexCount;
    m_meshBuffer->write_index(m_meshBufferHandle, data, size, offset);
    m_indexCount += size / indexSize;
}

void DynamicMesh::flush() {
    m_meshBuffer->flush();
}

void DynamicMesh::clear() {
    m_vertexCounts.fill(0);
    m_indexCount = 0;
}

void DynamicMesh::draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) {
    m_meshBuffer->bind(commandBuffer);
    if (m_indexType != duk::rhi::IndexType::NONE && m_indexCount > 0) {
        commandBuffer->draw_indexed(m_indexCount, instanceCount, m_firstIndex, static_cast<int32_t>(m_firstVertex), firstInstance);
    } else if (m_indexType == duk::rhi::IndexType::NONE && m_vertexCounts[duk::renderer::VertexAttributes::POSITION] > 0) {
        commandBuffer->draw(m_vertexCounts[duk::renderer::VertexAttributes::POSITION], instanceCount, m_firstVertex, firstInstance);
    }
}

}// namespace duk::renderer