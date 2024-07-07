//
// Created by Ricardo on 02/06/2024.
//
#include <duk_engine/systems/sprite_system.h>
#include <duk_engine/engine.h>

#include <duk_renderer/components/sprite_renderer.h>

namespace duk::engine {

void SpriteUpdateSystem::attach() {
}

void SpriteUpdateSystem::enter() {
}

void SpriteUpdateSystem::update() {
    const auto renderer = global<duk::renderer::Renderer>();
    for (auto object: all_objects_with<duk::renderer::SpriteRenderer>()) {
        duk::renderer::update_sprite_renderer(renderer, object.component<duk::renderer::SpriteRenderer>());
    }
}

void SpriteUpdateSystem::exit() {
}

}// namespace duk::engine
