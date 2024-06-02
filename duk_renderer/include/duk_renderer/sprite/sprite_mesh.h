//
// Created by Ricardo on 24/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_MESH_H
#define DUK_RENDERER_SPRITE_MESH_H

#include <duk_renderer/sprite/sprite.h>

#include <duk_renderer/mesh/mesh.h>

namespace duk::renderer {

class MeshBufferPool;
class MeshBuffer;

struct SpriteMeshCreateInfo {
    MeshBufferPool* meshBufferPool;
    const Sprite* sprite;
    uint32_t spriteIndex;
};

class SpriteMesh : public Mesh {
public:
    explicit SpriteMesh(const SpriteMeshCreateInfo& spriteMeshCreateInfo);

    ~SpriteMesh() override;

    void draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) override;

private:
    MeshBuffer* m_meshBuffer;
    uint32_t m_meshHandle;
    uint32_t m_firstVertex;
    uint32_t m_firstIndex;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_MESH_H
