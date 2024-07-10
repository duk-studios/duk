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

duk::hash::Hash calculate_mesh_hash(Sprite* altas, uint32_t index) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, altas);
    duk::hash::hash_combine(hash, index);
    return hash;
}

duk::hash::Hash calculate_material_hash(Sprite* altas) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, altas);
    return hash;
}

}// namespace detail

std::shared_ptr<Material> SpriteCache::material_for(duk::tools::Globals* globals, Sprite* atlas) {
    const auto hash = detail::calculate_material_hash(atlas);
    if (const auto it = m_materials.find(hash); it != m_materials.end()) {
        return it->second;
    }

    auto renderer = globals->get<Renderer>();
    auto builtins = globals->get<Builtins>();

    auto material = create_color_material(renderer, builtins, true);

    material->set("uBaseColor", atlas->image(), {duk::rhi::Sampler::Filter::NEAREST, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE});

    m_materials.emplace(hash, material);

    return material;
}

std::shared_ptr<SpriteMesh> SpriteCache::mesh_for(duk::tools::Globals* globals, Sprite* atlas, uint32_t index) {
    const auto hash = detail::calculate_mesh_hash(atlas, index);
    if (const auto it = m_meshes.find(hash); it != m_meshes.end()) {
        return it->second;
    }

    auto renderer = globals->get<Renderer>();

    SpriteMeshCreateInfo spriteMeshCreateInfo = {};
    spriteMeshCreateInfo.sprite = atlas;
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
