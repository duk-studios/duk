//
// Created by rov on 11/17/2023.
//
#include <duk_engine/systems/render_system.h>
#include <duk_engine/engine.h>

namespace duk::engine {

void RenderSystem::enter(Engine& engine) {

}

void RenderSystem::update(Engine& engine) {
    auto window = engine.window();
    if (!window || window->minimized()) {
        return;
    }

    auto scene = engine.scene();
    if (!scene) {
        return;
    }

    auto renderer = engine.renderer();
    if (!renderer) {
        return;
    }

    renderer->render(scene);
}

void RenderSystem::exit(Engine& engine) {

}
}
