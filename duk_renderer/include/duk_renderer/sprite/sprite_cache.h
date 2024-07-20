//
// Created by Ricardo on 23/04/2024.
//

#ifndef DUK_RENDERER_SPRITE_CACHE_H
#define DUK_RENDERER_SPRITE_CACHE_H

#include <duk_renderer/sprite/sprite.h>

#include "duk_tools/globals.h"

namespace duk::renderer {

class Material;
class SpriteMesh;

class SpriteCache {
public:
    std::shared_ptr<Material> material_for(duk::tools::Globals* globals, Sprite* atlas);

    std::shared_ptr<SpriteMesh> mesh_for(duk::tools::Globals* globals, Sprite* atlas, uint32_t index);

    void clear();

private:
    std::unordered_map<duk::hash::Hash, std::shared_ptr<Material>> m_materials;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<SpriteMesh>> m_meshes;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_CACHE_H
