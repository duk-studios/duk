//
// Created by Ricardo on 26/03/2024.
//
#include <duk_renderer/systems/transform_system.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::renderer {

namespace detail {

static size_t calculate_hash(const Transform& transform) {
    size_t hash = 0;
    duk::hash::hash_combine(hash, transform.position);
    duk::hash::hash_combine(hash, transform.rotation);
    duk::hash::hash_combine(hash, transform.scale);
    return hash;
}

static glm::mat4 calculate_model_matrix(const Transform& transform) {
    glm::mat4 model(1);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(transform.rotation);
    model = glm::scale(model, transform.scale);
    return model;
}

static void update_matrices(const Transform& transform, Matrices& matrices, const glm::mat4& parentModel) {
    matrices.model = parentModel * calculate_model_matrix(transform);
    matrices.invModel = glm::inverse(matrices.model);
}

static void update_child_transforms(TransformUpdateSystem* system, const duk::objects::ComponentHierarchyView<Transform>& transforms, const glm::mat4& parentModel, bool update) {
    for (auto [transform]: transforms) {
        const auto matrices = transform.component<Matrices>();
        const auto hash = calculate_hash(*transform);
        const auto changed = update || hash != matrices->hash;

        if (changed) {
            update_matrices(*transform, *matrices, parentModel);
            matrices->hash = hash;
        }

        update_child_transforms(system, system->child_components_of<Transform>(transform.id()), matrices->model, changed);

        auto materialSlot = transform.component<MaterialSlot>();
        if (!materialSlot || !changed) {
            continue;
        }

        if (auto material = materialSlot->material) {
            material->set(transform.id(), "uTransform", "model", matrices->model);
            material->set(transform.id(), "uTransform", "invModel", matrices->invModel);
            material->set("uCamera", system->global<Renderer>()->global_descriptors()->camera_ubo()->descriptor());
        }
    }
}

}// namespace detail

void TransformUpdateSystem::attach() {
    listen_component<TransformEnterEvent>(this);
}

void TransformUpdateSystem::update() {
    detail::update_child_transforms(this, root_components_of<Transform>(), glm::mat4(1), false);
}

void TransformUpdateSystem::receive(const TransformEnterEvent& event) {
    // TODO: take parent transform into account
    // might need to rethink how we query the world position and this kind of stuff.
    detail::update_matrices(*event.component, *event.component.component_or_add<Matrices>(), {});
}

}// namespace duk::renderer