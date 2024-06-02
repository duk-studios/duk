//
// Created by Ricardo on 02/06/2024.
//
#include <duk_engine/systems/sprite_system.h>
#include <duk_engine/engine.h>

#include <duk_renderer/components/sprite_renderer.h>

namespace duk::engine {
SpriteUpdateSystem::SpriteUpdateSystem()
    : System(kGameplayGroup) {
}

void SpriteUpdateSystem::enter(duk::objects::Objects& objects, Engine& engine) {
}

void SpriteUpdateSystem::update(duk::objects::Objects& objects, Engine& engine) {
    for (auto object: objects.all_with<duk::renderer::SpriteRenderer>()) {
        duk::renderer::update_sprite_renderer(engine.renderer(), object.component<duk::renderer::SpriteRenderer>());
    }
}

void SpriteUpdateSystem::exit(duk::objects::Objects& objects, Engine& engine) {
}

}// namespace duk::engine
