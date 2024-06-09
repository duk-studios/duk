/// 11/10/2023
/// empty_mesh.cpp

#include <duk_renderer/mesh/empty_mesh.h>

namespace duk::renderer {

EmptyMesh::EmptyMesh(const EmptyMeshCreateInfo& emptyMeshCreateInfo)
    : m_vertexCount(emptyMeshCreateInfo.vertexCount) {
}

const MeshBuffer* EmptyMesh::buffer() const {
    return nullptr;
}

uint32_t EmptyMesh::vertex_count() const {
    return m_vertexCount;
}

uint32_t EmptyMesh::vertex_offset() const {
    return 0;
}

uint32_t EmptyMesh::index_count() const {
    return 0;
}

uint32_t EmptyMesh::index_offset() const {
    return 0;
}

}// namespace duk::renderer