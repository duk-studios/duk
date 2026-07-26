//
// Created by Ricardo on 23/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_CACHE_H
#define DUK_RENDERER_SPRITE_CACHE_H

#include <duk_renderer/sprite/sprite.h>

namespace duk::renderer {

class MeshBufferPool;
class Mesh;

class SpriteCache {
public:
    explicit SpriteCache(std::shared_ptr<MeshBufferPool> meshBuffer);

    const Mesh* mesh_for(rhi::CommandContext& commandContext, const SpriteMetrics& spriteMetrics);

    void clear();

private:
    std::shared_ptr<MeshBufferPool> m_meshBufferPool;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<Mesh>> m_meshes;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_CACHE_H
