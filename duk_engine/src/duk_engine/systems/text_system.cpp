//
// Created by Ricardo on 05/04/2024.
//
#include <duk_engine/systems/text_system.h>
#include <duk_engine/engine.h>

#include <duk_renderer/components/text_renderer.h>

namespace duk::engine {

void TextUpdateSystem::enter(objects::Objects& objects, Engine& engine) {

}

void TextUpdateSystem::update(objects::Objects& objects, Engine& engine) {
    for (auto object : objects.all_with<duk::renderer::TextRenderer>()) {
        auto textRenderer = object.component<duk::renderer::TextRenderer>();
        duk::renderer::update_text_renderer(engine.renderer(), textRenderer);
    }
}

void TextUpdateSystem::exit(objects::Objects& objects, Engine& engine) {

}

}
