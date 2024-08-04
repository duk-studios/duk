//
// Created by Ricardo on 08/06/2024.
//

#ifndef DUK_UI_TEXT_MESH_CACHE_H
#define DUK_UI_TEXT_MESH_CACHE_H

#include <duk_hash/hash.h>
#include <unordered_map>
#include <memory>

#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_renderer/material/material.h>
#include <duk_ui/font/font_atlas.h>

namespace duk::ui {

class TextCache {
public:
    std::shared_ptr<duk::renderer::DynamicMesh> find_mesh(duk::hash::Hash hash) const;

    std::shared_ptr<duk::renderer::Material> find_material(duk::tools::Globals* globals, const FontAtlas* atlas, bool worldSpace);

    void store(duk::hash::Hash hash, const std::shared_ptr<duk::renderer::DynamicMesh>& mesh);

    void clear();

private:
    std::unordered_map<duk::hash::Hash, std::shared_ptr<duk::renderer::DynamicMesh>> m_meshes;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<duk::renderer::Material>> m_materials;
};

}// namespace duk::ui

#endif//DUK_UI_TEXT_MESH_CACHE_H
