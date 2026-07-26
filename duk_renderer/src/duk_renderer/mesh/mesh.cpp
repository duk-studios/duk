/// 11/10/2023
/// brush.cpp

#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>

namespace duk::renderer {

Mesh::Mesh(MeshBuffer& meshBuffer, uint32_t handle, uint32_t vertexCount, uint32_t indexCount)
    : m_meshBuffer(meshBuffer)
    , m_handle(handle)
    , m_vertexCount(vertexCount)
    , m_indexCount(indexCount) {
}

Mesh::~Mesh() {
    m_meshBuffer.free(m_handle);
}

const MeshBuffer& Mesh::buffer() const {
    return m_meshBuffer;
}

uint32_t Mesh::handle() const {
    return m_handle;
}

uint32_t Mesh::vertex_count() const {
    return m_vertexCount;
}

uint32_t Mesh::vertex_offset() const {
    return m_meshBuffer.first_vertex(m_handle);
}

uint32_t Mesh::index_count() const {
    return m_indexCount;
}

uint32_t Mesh::index_offset() const {
    return m_meshBuffer.first_index(m_handle);
}

}// namespace duk::renderer