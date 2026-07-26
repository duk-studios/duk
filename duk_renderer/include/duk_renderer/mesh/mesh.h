/// 11/10/2023
/// mesh.h

#ifndef DUK_RENDERER_MESH_H
#define DUK_RENDERER_MESH_H

#include <duk_resource/handle.h>

#include <cstdint>

namespace duk::renderer {

class MeshBuffer;

class Mesh {
public:
    Mesh(MeshBuffer& meshBuffer, uint32_t handle, uint32_t vertexCount, uint32_t indexCount);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept = delete;
    Mesh& operator=(Mesh&& other) noexcept = delete;

    ~Mesh();

    DUK_NO_DISCARD const MeshBuffer& buffer() const;

    DUK_NO_DISCARD uint32_t handle() const;

    DUK_NO_DISCARD uint32_t vertex_count() const;

    DUK_NO_DISCARD uint32_t vertex_offset() const;

    DUK_NO_DISCARD uint32_t index_count() const;

    DUK_NO_DISCARD uint32_t index_offset() const;

private:
    MeshBuffer& m_meshBuffer;
    uint32_t m_handle;
    uint32_t m_vertexCount;
    uint32_t m_indexCount;
};

using MeshResource = duk::resource::Handle<Mesh>;

}// namespace duk::renderer

#endif// DUK_RENDERER_MESH_H
