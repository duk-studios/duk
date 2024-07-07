//
// Created by Ricardo on 26/03/2024.
//
#include <duk_engine/systems/transform_system.h>
#include <duk_renderer/components/transform.h>

namespace duk::engine {

void TransformUpdateSystem::enter() {
    duk::renderer::update_transforms(*objects());
}

void TransformUpdateSystem::update() {
    duk::renderer::update_transforms(*objects());
}

}// namespace duk::engine