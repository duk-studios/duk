//
// Created by Ricardo on 23/04/2024.
//
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/sprite/sprite_mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/material/material.h>

namespace duk::renderer {

namespace detail {

duk::hash::Hash calculate_mesh_hash(const Sprite& sprite, uint32_t index) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, &sprite);
    duk::hash::hash_combine(hash, index);
    return hash;
}

duk::hash::Hash calculate_material_hash(const Sprite& sprite) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, &sprite);
    return hash;
}

}// namespace detail

std::shared_ptr<Material> SpriteCache::material_for(const duk::tools::Globals& globals, const Sprite& sprite) {
    const auto hash = detail::calculate_material_hash(sprite);
    if (const auto it = m_materials.find(hash); it != m_materials.end()) {
        return it->second;
    }

    auto renderer = globals.get<Renderer>();
    auto builtins = globals.get<Builtins>();

    auto material = create_color_material(renderer, builtins, true);

    material->set("uBaseColor", sprite.image(), {duk::rhi::Sampler::Filter::NEAREST, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE});

    m_materials.emplace(hash, material);

    return material;
}

std::shared_ptr<SpriteMesh> SpriteCache::mesh_for(const duk::tools::Globals& globals, const Sprite& sprite, uint32_t index) {
    const auto hash = detail::calculate_mesh_hash(sprite, index);
    if (const auto it = m_meshes.find(hash); it != m_meshes.end()) {
        return it->second;
    }

    auto renderer = globals.get<Renderer>();

    SpriteMeshCreateInfo spriteMeshCreateInfo = {};
    spriteMeshCreateInfo.sprite = &sprite;
    spriteMeshCreateInfo.spriteIndex = index;
    spriteMeshCreateInfo.meshBufferPool = renderer->mesh_buffer_pool();

    auto mesh = std::make_shared<SpriteMesh>(spriteMeshCreateInfo);

    m_meshes.emplace(hash, mesh);

    return mesh;
}

void SpriteCache::clear() {
    m_meshes.clear();
    m_materials.clear();
}

}// namespace duk::renderer
