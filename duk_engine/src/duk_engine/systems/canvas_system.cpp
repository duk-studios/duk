//
// Created by Ricardo on 07/04/2024.
//

#include <duk_engine/systems/canvas_system.h>
#include <duk_engine/engine.h>

namespace duk::engine {

void CanvasUpdateSystem::attach() {
    listen_component<CanvasEnterEvent>(this);
}

void CanvasUpdateSystem::update() {
    auto [canvas] = first_components_of<duk::renderer::Canvas>();

    if (!canvas) {
        duk::log::warn("No object with Canvas component found, skipping CanvasUpdateSystem::update");
        return;
    }

    for (auto [canvasTransform]: all_components_of<duk::renderer::CanvasTransform>()) {
        duk::renderer::update_canvas_transform(canvas, canvasTransform);
    }
}

void CanvasUpdateSystem::receive(const CanvasEnterEvent& event) {
    const auto window = global<duk::platform::Window>();
    duk::renderer::update_canvas(event.component, window->width(), window->height());
}

}// namespace duk::engine
