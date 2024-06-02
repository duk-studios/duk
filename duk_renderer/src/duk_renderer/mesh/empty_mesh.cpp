/// 11/10/2023
/// empty_mesh.cpp

#include <duk_renderer/mesh/empty_mesh.h>

namespace duk::renderer {

EmptyMesh::EmptyMesh(const EmptyMeshCreateInfo& emptyMeshCreateInfo)
    : m_vertexCount(emptyMeshCreateInfo.vertexCount) {
}

uint32_t EmptyMesh::vertex_count() const {
    return m_vertexCount;
}

void EmptyMesh::draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) {
    commandBuffer->draw(m_vertexCount, instanceCount, 0, firstInstance);
}

}// namespace duk::renderer