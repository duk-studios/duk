//
// Created by Ricardo on 07/04/2024.
//

#include <duk_ui/systems/canvas_system.h>
#include <duk_ui/components/canvas.h>

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/renderer.h>

namespace duk::ui {

namespace detail {

static uint64_t calculate_canvas_hash(uint32_t width, uint32_t height) {
    uint64_t hash = 0;
    duk::hash::hash_combine(hash, width);
    duk::hash::hash_combine(hash, height);
    return hash;
}

static void update_canvas(CanvasUpdateSystem* system, const objects::Component<Canvas>& canvas, uint32_t width, uint32_t height) {
    const auto hash = calculate_canvas_hash(width, height);
    if (hash != canvas->hash) {
        canvas->size = glm::ivec2(width, height);
        canvas->ubo->proj = glm::ortho(0.f, (float)width, 0.f, (float)height);
        canvas->ubo->view = glm::mat4(1);
        canvas->ubo->invView = glm::mat4(1);
        canvas->ubo->vp = canvas->ubo->proj;
        if (!canvas->ubo.valid()) {
            const auto renderer = system->global<duk::renderer::Renderer>();
            canvas->ubo.create(renderer->rhi(), renderer->main_command_queue());
        }
        canvas->ubo.flush();
        canvas->hash = hash;
    }
}

static void update_canvas_transform(CanvasUpdateSystem* system, const duk::objects::Component<Canvas>& canvas, const duk::objects::ComponentHierarchyView<CanvasTransform>& canvasTransforms) {
    for (auto [canvasTransform]: canvasTransforms) {
        const auto anchorPosition = canvasTransform->anchor * glm::vec2(canvas->size);
        const auto position = canvasTransform->position + anchorPosition;
        const auto halfSize = canvasTransform->size / 2.0f;
        const auto pivotOffset = halfSize * ((canvasTransform->pivot * 2.0f) - 1.0f);
        auto model = glm::translate(glm::mat4(1), glm::vec3(position - pivotOffset, 0.0));
        model = glm::scale(model, glm::vec3(canvasTransform->scale, 1));
        canvasTransform->model = model;

        update_canvas_transform(system, canvas, system->child_components_of<CanvasTransform>(canvasTransform.id()));

        auto materialSlot = canvasTransform.component<duk::renderer::MaterialSlot>();
        if (!materialSlot) {
            return;
        }

        if (auto material = materialSlot->material) {
            material->set(canvasTransform.id(), "uTransform", "model", canvasTransform->model);
            material->set("uCamera", canvas->ubo.descriptor());
        }
    }
}

}// namespace detail

void CanvasUpdateSystem::update() {
    const auto renderer = global<duk::renderer::Renderer>();
    for (auto [canvas]: all_components_of<Canvas>()) {
        detail::update_canvas(this, canvas, renderer->render_width(), renderer->render_height());

        detail::update_canvas_transform(this, canvas, child_components_of<CanvasTransform>(canvas.id()));
    }
}

}// namespace duk::ui
