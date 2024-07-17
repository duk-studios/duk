//
// Created by Ricardo on 05/04/2024.
//

#include <duk_renderer/systems/text_system.h>
#include <duk_renderer/components/text_renderer.h>

namespace duk::renderer {

void TextUpdateSystem::attach() {
    listen_component<TextRendererEnterEvent>(this);
    listen_component<TextRendererExitEvent>(this);
}

void TextUpdateSystem::receive(const TextRendererEnterEvent& event) {
    duk::renderer::update_text_renderer(globals(), event.component);
}

void TextUpdateSystem::receive(const TextRendererExitEvent& event) {
}

void TextUpdateSystem::update() {
    for (auto [textRenderer]: all_components_of<duk::renderer::TextRenderer>()) {
        duk::renderer::update_text_renderer(globals(), textRenderer);
    }
}

}// namespace duk::renderer
