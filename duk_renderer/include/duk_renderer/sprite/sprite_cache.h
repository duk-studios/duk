//
// Created by Ricardo on 23/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_CACHE_H
#define DUK_RENDERER_SPRITE_CACHE_H

#include <duk_renderer/sprite/sprite.h>

namespace duk::renderer {

class Material;
class SpriteMesh;
class MeshBufferPool;

struct SpriteCacheCreateInfo {
    Renderer* renderer;
};

class SpriteCache {
public:
    explicit SpriteCache(const SpriteCacheCreateInfo& spriteCacheCreateInfo);

    ~SpriteCache();

    std::shared_ptr<Material> material_for(Sprite* atlas, uint32_t index);

    std::shared_ptr<SpriteMesh> mesh_for(Sprite* atlas, uint32_t index);

private:
    Renderer* m_renderer;
    std::unique_ptr<MeshBufferPool> m_meshBufferPool;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<Material>> m_materials;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<SpriteMesh>> m_meshes;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_CACHE_H
