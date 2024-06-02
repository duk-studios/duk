/// 11/10/2023
/// mesh.h

#ifndef DUK_RENDERER_MESH_H
#define DUK_RENDERER_MESH_H

#include <duk_rhi/command/command_buffer.h>

#include <duk_resource/resource.h>

namespace duk::renderer {

class Mesh {
public:
    virtual ~Mesh();

    virtual void draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) = 0;
};

using MeshResource = duk::resource::Handle<Mesh>;

}// namespace duk::renderer

#endif// DUK_RENDERER_MESH_H
