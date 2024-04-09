//
// Created by Ricardo on 07/04/2024.
//

#include <duk_engine/systems/canvas_system.h>
#include <duk_engine/engine.h>

#include <duk_renderer/components/canvas.h>

namespace duk::engine {

namespace detail {

static void update_canvas(duk::objects::Objects& objects, uint32_t width, uint32_t height) {
    for (auto object : objects.all_with<duk::renderer::Canvas>()) {
        duk::renderer::update_canvas(object.component<duk::renderer::Canvas>(), width, height);
    }
}

}

void CanvasUpdateSystem::enter(duk::objects::Objects& objects, Engine& engine) {
    m_listener.listen(engine.window()->window_resize_event, [&objects](uint32_t width, uint32_t height) {
        detail::update_canvas(objects, width, height);
    });
    detail::update_canvas(objects, engine.window()->width(), engine.window()->height());
}

void CanvasUpdateSystem::update(duk::objects::Objects& objects, Engine& engine) {

    auto canvasObject = objects.first_with<duk::renderer::Canvas>();

    if (!canvasObject) {
        duk::log::warn("No object with Canvas component found, skipping CanvasUpdateSystem::update");
        return;
    }

    auto canvas = canvasObject.component<duk::renderer::Canvas>();

    for (auto object : objects.all_with<duk::renderer::CanvasTransform>()) {
        auto canvasTransform = object.component<duk::renderer::CanvasTransform>();
        duk::renderer::update_canvas_transform(canvas, canvasTransform);
    }
}

void CanvasUpdateSystem::exit(duk::objects::Objects& objects, Engine& engine) {

}

}
