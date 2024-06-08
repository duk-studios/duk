//
// Created by Ricardo on 07/06/2024.
//

#include <duk_renderer/text/text_mesh_cache.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::renderer {

namespace detail {

static duk::hash::Hash calculate_material_hash(const FontAtlas* atlas, bool worldSpace) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, atlas);
    duk::hash::hash_combine(hash, worldSpace);
    return hash;
}

}// namespace detail

std::shared_ptr<DynamicMesh> TextCache::find_mesh(duk::hash::Hash hash) const {
    if (const auto it = m_meshes.find(hash); it != m_meshes.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Material> TextCache::find_material(Renderer* renderer, const FontAtlas* atlas, bool worldSpace) {
    auto hash = detail::calculate_material_hash(atlas, worldSpace);
    if (const auto it = m_materials.find(hash); it != m_materials.end()) {
        return it->second;
    }

    auto material = create_text_material(renderer);
    material->set("uBaseColor", atlas->image(), kDefaultTextureSampler);

    auto globalDescriptors = renderer->global_descriptors();
    auto ubo = worldSpace ? globalDescriptors->camera_ubo() : globalDescriptors->canvas_ubo();
    material->set("uCamera", ubo->descriptor());

    m_materials[hash] = material;

    return material;
}

void TextCache::store(duk::hash::Hash hash, const std::shared_ptr<DynamicMesh>& mesh) {
    m_meshes[hash] = mesh;
}

void TextCache::clear() {
    m_meshes.clear();
}

}// namespace duk::renderer
