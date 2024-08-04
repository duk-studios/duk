//
// Created by Ricardo on 05/04/2024.
//

#include <duk_ui/systems/text_system.h>
#include <duk_ui/components/text_renderer.h>

namespace duk::ui {

void TextUpdateSystem::attach() {
    listen_component<TextRendererEnterEvent>(this);
}

void TextUpdateSystem::receive(const TextRendererEnterEvent& event) {
    duk::ui::update_text_renderer(globals(), event.component);
}

void TextUpdateSystem::update() {
    for (auto [textRenderer]: all_components_of<duk::ui::TextRenderer>()) {
        duk::ui::update_text_renderer(globals(), textRenderer);
    }
}

}// namespace duk::ui
