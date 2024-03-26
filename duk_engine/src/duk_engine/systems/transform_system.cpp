//
// Created by Ricardo on 26/03/2024.
//
#include <duk_engine/systems/transform_system.h>
#include <duk_renderer/components/transform.h>

namespace duk::engine {

void TransformUpdateSystem::enter(duk::scene::Objects& objects, Engine& engine) {
    duk::renderer::update_transforms(objects);
}

void TransformUpdateSystem::update(duk::scene::Objects& objects, Engine& engine) {
    duk::renderer::update_transforms(objects);
}

void TransformUpdateSystem::exit(duk::scene::Objects& objects, Engine& engine) {
}

}// namespace duk::engine