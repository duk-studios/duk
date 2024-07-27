//
// Created by Ricardo on 26/03/2024.
//
#include <duk_renderer/systems/transform_system.h>

#include <duk_renderer/components/material_slot.h>

namespace duk::renderer {

namespace detail {

static glm::mat4 calculate_model(const Transform& transform) {
    glm::mat4 model(1);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(transform.rotation);
    model = glm::scale(model, transform.scale);
    return model;
}

static void update_transform(Transform& transform, const glm::mat4& parentModel) {
    transform.model = parentModel * calculate_model(transform);
    transform.invModel = glm::inverse(transform.model);
}

static void update_child_transforms(TransformUpdateSystem* system, const duk::objects::ComponentHierarchyView<Transform>& transforms, const glm::mat4& parentModel) {
    for (auto [transform]: transforms) {
        update_transform(*transform, parentModel);

        update_child_transforms(system, system->child_components_of<Transform>(transform.id()), transform->model);

        auto materialSlot = transform.component<MaterialSlot>();
        if (!materialSlot) {
            continue;
        }

        if (auto material = materialSlot->material) {
            material->set(transform.id(), "uTransform", "model", transform->model);
            material->set(transform.id(), "uTransform", "invModel", transform->invModel);
        }
    }
}

}// namespace detail

void TransformUpdateSystem::attach() {
    listen_component<TransformEnterEvent>(this);
}

void TransformUpdateSystem::update() {
    detail::update_child_transforms(this, root_components_of<Transform>(), glm::mat4(1));
}

void TransformUpdateSystem::receive(const TransformEnterEvent& event) {
    detail::update_transform(*event.component, {});
}

}// namespace duk::renderer