//
// Created by Ricardo on 26/03/2024.
//

#include <duk_engine/engine.h>
#include <duk_engine/systems/camera_system.h>
#include <duk_renderer/components/camera.h>

namespace duk::engine {

void CameraUpdateSystem::enter() {
    auto renderer = global<duk::renderer::Renderer>();
    duk::renderer::update_cameras(*objects(), renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::update() {
    auto renderer = global<duk::renderer::Renderer>();
    duk::renderer::update_cameras(*objects(), renderer->render_width(), renderer->render_height());
}

}// namespace duk::engine