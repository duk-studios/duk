//
// Created by Ricardo on 07/06/2024.
//

#ifndef DUK_UI_TEXT_MESH_H
#define DUK_UI_TEXT_MESH_H

#include <duk_ui/font/font_atlas.h>
#include <duk_ui/text/text_alignment.h>
#include <duk_ui/text/text_mesh_cache.h>

namespace duk::ui {

struct TextMeshCreateInfo {
    duk::renderer::MeshBufferPool* meshBufferPool;
    TextCache* textCache;
    bool dynamic;
};

class TextMesh {
public:
    TextMesh(const TextMeshCreateInfo& textMeshCreateInfo);

    bool update(const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, float pixelsPerUnit);

    std::shared_ptr<duk::renderer::DynamicMesh> mesh() const;

private:
    duk::renderer::MeshBufferPool* m_meshBufferPool;
    TextCache* m_textCache;
    bool m_dynamic;
    uint32_t m_textCount;
    duk::hash::Hash m_hash;
    std::shared_ptr<duk::renderer::DynamicMesh> m_mesh;
};

}// namespace duk::ui

#endif//DUK_UI_TEXT_MESH_H
