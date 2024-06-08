//
// Created by Ricardo on 08/06/2024.
//

#ifndef DUK_RENDERER_TEXT_MESH_CACHE_H
#define DUK_RENDERER_TEXT_MESH_CACHE_H

#include <duk_hash/hash.h>
#include <unordered_map>
#include <memory>

#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_renderer/material/material.h>
#include <duk_renderer/font/font_atlas.h>

namespace duk::renderer {

class TextCache {
public:
    std::shared_ptr<DynamicMesh> find_mesh(duk::hash::Hash hash) const;

    std::shared_ptr<Material> find_material(Renderer* renderer, const FontAtlas* atlas, bool worldSpace);

    void store(duk::hash::Hash hash, const std::shared_ptr<DynamicMesh>& mesh);

    void clear();

private:
    std::unordered_map<duk::hash::Hash, std::shared_ptr<DynamicMesh>> m_meshes;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<Material>> m_materials;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_TEXT_MESH_CACHE_H
