//
// Created by Ricardo on 08/06/2024.
//

#ifndef DUK_RENDERER_TEXT_MESH_BUILDER_H
#define DUK_RENDERER_TEXT_MESH_BUILDER_H

#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_renderer/font/font_atlas.h>
#include <duk_renderer/text/text_alignment.h>

namespace duk::renderer {

std::shared_ptr<DynamicMesh> allocate_text_mesh(MeshBufferPool* meshBufferPool, uint32_t textCount, bool dynamic);

void build_text_mesh(DynamicMesh* mesh, const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, float lineHeight, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, float pixelsPerUnit);

}// namespace duk::renderer

#endif//DUK_RENDERER_TEXT_MESH_BUILDER_H
