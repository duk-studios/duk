//
// Created by Ricardo on 05/04/2024.
//

#include <duk_ui/components/text.h>
#include <duk_renderer/renderer.h>

#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/canvas.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>

#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::ui {

void update_text_renderer(duk::tools::Globals* globals, const objects::Component<Text>& textRenderer) {
    auto renderer = globals->get<duk::renderer::Renderer>();

    Font::BuildAtlasParams buildAtlasParams = {};
    buildAtlasParams.rhi = renderer->rhi();
    buildAtlasParams.commandQueue = renderer->main_command_queue();
    buildAtlasParams.fontSize = textRenderer->fontSize;

    auto atlas = textRenderer->font->atlas(buildAtlasParams);

    auto [meshSlot, materialSlot] = textRenderer.components_or_add<duk::renderer::MeshSlot, duk::renderer::MaterialSlot>();

    auto textCache = renderer->text_cache();

    const auto transform = textRenderer.component<duk::renderer::Transform>();
    if (!materialSlot->material) {
        materialSlot->material = textCache->find_material(globals, atlas, transform.valid());
    }

    if (!meshSlot->mesh) {
        TextMeshCreateInfo textMeshCreateInfo = {};
        textMeshCreateInfo.textCache = renderer->text_cache();
        textMeshCreateInfo.meshBufferPool = renderer->mesh_buffer_pool();
        textMeshCreateInfo.dynamic = textRenderer->dynamic;
        textRenderer->mesh = std::make_shared<TextMesh>(textMeshCreateInfo);
    }

    auto updated = textRenderer->mesh->update(textRenderer->text, atlas, textRenderer->fontSize, textRenderer->size, textRenderer->vertAlignment, textRenderer->horiAlignment, transform ? 100 : 1);

    if (updated) {
        meshSlot->mesh = textRenderer->mesh->mesh();
    }
}

}// namespace duk::ui
