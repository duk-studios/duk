//
// Created by Ricardo on 08/06/2024.
//

#ifndef DUK_UI_TEXT_MESH_BUILDER_H
#define DUK_UI_TEXT_MESH_BUILDER_H

#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_ui/font/font_atlas.h>
#include <duk_ui/text/text_alignment.h>

namespace duk::ui {

std::shared_ptr<duk::renderer::DynamicMesh> allocate_text_mesh(duk::renderer::MeshBufferPool* meshBufferPool, uint32_t textCount, bool dynamic);

void build_text_mesh(duk::renderer::DynamicMesh* mesh, const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment);

}// namespace duk::ui

#endif//DUK_UI_TEXT_MESH_BUILDER_H
