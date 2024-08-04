//
// Created by Ricardo on 07/06/2024.
//

#include <duk_ui/text/text_mesh_cache.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::ui {

namespace detail {

static duk::hash::Hash calculate_material_hash(const FontAtlas* atlas, bool worldSpace) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, atlas);
    duk::hash::hash_combine(hash, worldSpace);
    return hash;
}

}// namespace detail

std::shared_ptr<duk::renderer::DynamicMesh> TextCache::find_mesh(duk::hash::Hash hash) const {
    if (const auto it = m_meshes.find(hash); it != m_meshes.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<duk::renderer::Material> TextCache::find_material(duk::tools::Globals* globals, const FontAtlas* atlas, bool worldSpace) {
    auto hash = detail::calculate_material_hash(atlas, worldSpace);
    if (const auto it = m_materials.find(hash); it != m_materials.end()) {
        return it->second;
    }

    const auto renderer = globals->get<duk::renderer::Renderer>();
    const auto builtins = globals->get<duk::renderer::Builtins>();

    auto material = create_text_material(renderer, builtins);
    material->set("uBaseColor", atlas->image(), duk::renderer::kDefaultTextureSampler);

    auto globalDescriptors = renderer->global_descriptors();
    auto ubo = worldSpace ? globalDescriptors->camera_ubo() : globalDescriptors->canvas_ubo();
    material->set("uCamera", ubo->descriptor());

    m_materials[hash] = material;

    return material;
}

void TextCache::store(duk::hash::Hash hash, const std::shared_ptr<duk::renderer::DynamicMesh>& mesh) {
    m_meshes[hash] = mesh;
}

void TextCache::clear() {
    m_meshes.clear();
    m_materials.clear();
}

}// namespace duk::ui
