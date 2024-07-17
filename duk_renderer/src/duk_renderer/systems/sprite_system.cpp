//
// Created by Ricardo on 02/06/2024.
//
#include <duk_renderer/systems/sprite_system.h>
#include <duk_renderer/components/sprite_renderer.h>

namespace duk::renderer {

void SpriteUpdateSystem::attach() {
    listen_component<SpriteRendererEnterEvent>(this);
    listen_component<SpriteRendererExitEvent>(this);
}

void SpriteUpdateSystem::update() {
    for (auto object: all_objects_with<duk::renderer::SpriteRenderer>()) {
        duk::renderer::update_sprite_renderer(globals(), object.component<duk::renderer::SpriteRenderer>());
    }
}

void SpriteUpdateSystem::receive(const SpriteRendererEnterEvent& event) {
    duk::renderer::update_sprite_renderer(globals(), event.component);
}

void SpriteUpdateSystem::receive(const SpriteRendererExitEvent& event) {
}
}// namespace duk::renderer
