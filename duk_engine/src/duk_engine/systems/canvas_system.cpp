//
// Created by Ricardo on 07/04/2024.
//

#include <duk_engine/systems/canvas_system.h>
#include <duk_engine/engine.h>

namespace duk::engine {

void CanvasUpdateSystem::enter(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    listen<duk::renderer::Canvas, duk::objects::ComponentEnterEvent>(this);
}

void CanvasUpdateSystem::update(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    auto canvasObject = objects.first_with<duk::renderer::Canvas>();

    if (!canvasObject) {
        duk::log::warn("No object with Canvas component found, skipping CanvasUpdateSystem::update");
        return;
    }

    auto canvas = canvasObject.component<duk::renderer::Canvas>();

    for (auto object: objects.all_with<duk::renderer::CanvasTransform>()) {
        auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
        duk::renderer::update_canvas_transform(canvas, canvasTransform);
    }
}

void CanvasUpdateSystem::exit(duk::objects::Objects& objects, duk::tools::Globals& globals) {
}

void CanvasUpdateSystem::receive(const CanvasEnterEvent& event) {
    auto window = event.globals.get<duk::platform::Window>();
    duk::renderer::update_canvas(event.component, window->width(), window->height());
}

}// namespace duk::engine
