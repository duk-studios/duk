//
// Created by Ricardo on 07/04/2024.
//

#include <duk_platform/window.h>
#include <duk_renderer/systems/canvas_system.h>

namespace duk::renderer {

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
}// namespace duk::renderer
