//
// Created by Ricardo on 26/03/2024.
//
#include <duk_renderer/systems/transform_system.h>

namespace duk::renderer {

void TransformUpdateSystem::attach() {
    listen_component<TransformEnterEvent>(this);
    listen_component<TransformExitEvent>(this);
}

void TransformUpdateSystem::update() {
    duk::renderer::update_transforms(*objects());
}

void TransformUpdateSystem::receive(const TransformEnterEvent& event) {
    duk::renderer::update_transform(event.component);
}

void TransformUpdateSystem::receive(const TransformExitEvent& event) {
}

}// namespace duk::renderer