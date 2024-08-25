//
// Created by Ricardo on 07/04/2024.
//

#include <duk_ui/systems/canvas_system.h>
#include <duk_ui/components/canvas.h>

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/transform.h>

namespace duk::ui {

namespace detail {

static uint64_t calculate_canvas_hash(uint32_t width, uint32_t height) {
    uint64_t hash = 0;
    duk::hash::hash_combine(hash, width);
    duk::hash::hash_combine(hash, height);
    return hash;
}

static void update_canvas_ubo(CanvasUpdateSystem* system, const objects::Component<Canvas>& canvas, uint32_t width, uint32_t height) {
    if (!canvas->viewProjUBO.valid()) {
        const auto renderer = system->global<duk::renderer::Renderer>();
        canvas->viewProjUBO.create(renderer->rhi(), renderer->main_command_queue());
    }
    canvas->viewProjUBO->proj = glm::ortho(0.f, (float)width, 0.f, (float)height);
    canvas->viewProjUBO->view = glm::mat4(1);
    canvas->viewProjUBO->invView = glm::mat4(1);
    canvas->viewProjUBO->vp = canvas->viewProjUBO->proj;
    canvas->viewProjUBO.flush();
}

static uint64_t calculate_canvas_transform_hash(const CanvasTransform& canvasTransform) {
    uint64_t hash = 0;
    duk::hash::hash_combine(hash, canvasTransform.anchorMin);
    duk::hash::hash_combine(hash, canvasTransform.anchorMax);
    duk::hash::hash_combine(hash, canvasTransform.anchorMinOffsets);
    duk::hash::hash_combine(hash, canvasTransform.anchorMaxOffsets);
    duk::hash::hash_combine(hash, canvasTransform.pivot);
    duk::hash::hash_combine(hash, canvasTransform.scale);
    return hash;
}

static void update_canvas_transform_matrices(const CanvasTransform& canvasTransform, CanvasTransformRect& matrices, const glm::mat4& parentModel, const glm::vec2& parentSize) {
    const auto parentMin = canvasTransform.anchorMinOffsets + canvasTransform.anchorMin * parentSize;
    const auto parentMax = canvasTransform.anchorMaxOffsets + canvasTransform.anchorMax * parentSize;
    const auto size = parentMax - parentMin;
    const auto center = parentMin + size * canvasTransform.pivot;
    matrices.size = size;
    matrices.model = glm::translate(parentModel, glm::vec3(center, 0.0));
    matrices.model = glm::scale(matrices.model, glm::vec3(canvasTransform.scale, 0.0));
    matrices.invModel = glm::inverse(matrices.model);
}

static void update_canvas_transforms(CanvasUpdateSystem* system, const duk::objects::Component<Canvas>& canvas, const glm::mat4& parentModel, const glm::vec2& parentSize, const duk::objects::ComponentHierarchyView<CanvasTransform>& canvasTransforms, bool update) {
    for (auto [canvasTransform]: canvasTransforms) {
        const auto matrices = canvasTransform.component_or_add<CanvasTransformRect>();
        const auto hash = calculate_canvas_transform_hash(*canvasTransform);
        const auto changed = hash != matrices->hash || update;
        if (changed) {
            update_canvas_transform_matrices(*canvasTransform, *matrices, parentModel, parentSize);
            matrices->hash = hash;
        }

        update_canvas_transforms(system, canvas, matrices->model, matrices->size, system->child_components_of<CanvasTransform>(canvasTransform.id()), changed);

        auto materialSlot = canvasTransform.component<duk::renderer::MaterialSlot>();
        if (!materialSlot || !changed) {
            continue;
        }

        if (auto material = materialSlot->material) {
            material->set(canvasTransform.id(), "uTransform", "model", matrices->model);
            material->set(canvasTransform.id(), "uTransform", "invModel", matrices->invModel);
            material->set("uCamera", canvas->viewProjUBO.descriptor());
        }
    }
}

}// namespace detail

void CanvasUpdateSystem::update() {
    const auto renderer = global<duk::renderer::Renderer>();
    for (auto [canvas]: all_components_of<Canvas>()) {
        const auto width = renderer->render_width();
        const auto height = renderer->render_height();
        const auto hash = detail::calculate_canvas_hash(width, height);
        if (hash != canvas->hash) {
            detail::update_canvas_ubo(this, canvas, width, height);
            canvas->hash = hash;
        }

        detail::update_canvas_transforms(this, canvas, glm::mat4(1), glm::vec2(width, height), child_components_of<CanvasTransform>(canvas.id()), false);
    }
}

}// namespace duk::ui
