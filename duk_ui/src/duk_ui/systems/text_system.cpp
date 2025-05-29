//
// Created by Ricardo on 05/04/2024.
//

#include <duk_ui/systems/text_system.h>
#include <duk_ui/components/text.h>
#include <duk_ui/text/text_mesh_builder.h>

#include <duk_renderer/renderer.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::ui {

namespace detail {

static duk::hash::Hash calculate_text_hash(const std::string_view& text, const Font* font, uint32_t fontSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, const glm::vec2& texBoxSize) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, font);
    duk::hash::hash_combine(hash, text);
    duk::hash::hash_combine(hash, fontSize);
    duk::hash::hash_combine(hash, vertAlignment);
    duk::hash::hash_combine(hash, horiAlignment);
    duk::hash::hash_combine(hash, texBoxSize);
    return hash;
}

static duk::hash::Hash calculate_material_hash(const FontAtlas* atlas, bool worldSpace, const glm::vec4& color) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, atlas);
    duk::hash::hash_combine(hash, worldSpace);
    duk::hash::hash_combine(hash, color);
    return hash;
}

static uint32_t calculate_text_capacity(const std::string_view& text, bool dynamic) {
    if (dynamic) {
        constexpr uint32_t kTextChunk = 64;
        return ((text.size() / kTextChunk) + 1) * kTextChunk;
    }
    return text.size();
}

}// namespace detail

void TextUpdateSystem::attach() {
    listen_component<TextRendererEnterEvent>(this);
    m_meshBufferPool = global<duk::renderer::Renderer>()->mesh_buffer_pool();
}

void TextUpdateSystem::update() {
    for (auto [text]: all_components_of<Text>()) {
        update_text(text);
    }
}

void TextUpdateSystem::receive(const TextRendererEnterEvent& event) {
    update_text(event.component);
}

void TextUpdateSystem::update_text(const duk::objects::Component<Text>& text) {
    const auto hash = detail::calculate_text_hash(text->text, text->font.get(), text->fontSize, text->vertAlignment, text->horiAlignment, text->size);
    if (text->hash == hash) {
        return;
    }
    text->hash = hash;

    auto renderer = global<duk::renderer::Renderer>();

    Font::BuildAtlasParams buildAtlasParams = {};
    buildAtlasParams.rhi = renderer->rhi();
    buildAtlasParams.commandQueue = renderer->main_command_queue();
    buildAtlasParams.fontSize = text->fontSize;

    auto atlas = text->font->atlas(buildAtlasParams);

    update_text_material(text, atlas);

    update_text_mesh(text, atlas);
}

void TextUpdateSystem::update_text_material(const duk::objects::Component<Text>& text, const FontAtlas* atlas) {
    const bool worldSpace = text.component<duk::renderer::Transform>().valid();

    auto hash = detail::calculate_material_hash(atlas, worldSpace, text->color);
    auto it = m_materials.find(hash);
    if (it == m_materials.end()) {
        const auto renderer = global<duk::renderer::Renderer>();
        const auto builtins = global<duk::renderer::Builtins>();

        auto material = create_text_material(renderer, builtins);
        material->set("uBaseColor", atlas->image(), duk::renderer::kDefaultTextureSampler);
        material->set("uProperties", "color", text->color);

        it = m_materials.emplace(hash, std::move(material)).first;
    }
    const auto materialSlot = text.component_or_add<duk::renderer::MaterialSlot>();
    materialSlot->material = it->second;
}

void TextUpdateSystem::update_text_mesh(const duk::objects::Component<Text>& text, const FontAtlas* atlas) {
    const auto meshSlot = text.component_or_add<duk::renderer::MeshSlot>();
    if (!text->dynamic) {
        auto it = m_meshes.find(text->hash);
        if (it != m_meshes.end()) {
            meshSlot->mesh = it->second;
            return;
        }
    }

    const auto textCount = detail::calculate_text_capacity(text->text, text->dynamic);

    auto mesh = meshSlot->mesh.as<duk::renderer::DynamicMesh>().shared();
    if (!mesh || text->capacity < textCount || !text->dynamic) {
        mesh = allocate_text_mesh(m_meshBufferPool, textCount, text->dynamic);
        text->capacity = textCount;
    }

    const auto pixelsPerUnit = text.component<duk::renderer::Transform>().valid() ? 100 : 1;

    build_text_mesh(mesh.get(), text->text, atlas, text->fontSize, text->size, text->vertAlignment, text->horiAlignment, pixelsPerUnit);

    if (!text->dynamic) {
        m_meshes.emplace(text->hash, mesh);
    }

    meshSlot->mesh = mesh;
}
}// namespace duk::ui
