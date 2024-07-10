/// 11/10/2023
/// mesh.h

#ifndef DUK_RENDERER_MESH_H
#define DUK_RENDERER_MESH_H

#include <duk_rhi/command/command_buffer.h>

#include <duk_resource/handle.h>

namespace duk::renderer {

class MeshBuffer;

class Mesh {
public:
    virtual ~Mesh();

    virtual const MeshBuffer* buffer() const = 0;

    virtual uint32_t vertex_count() const = 0;

    virtual uint32_t vertex_offset() const = 0;

    virtual uint32_t index_count() const = 0;

    virtual uint32_t index_offset() const = 0;
};

using MeshResource = duk::resource::Handle<Mesh>;

}// namespace duk::renderer

#endif// DUK_RENDERER_MESH_H
