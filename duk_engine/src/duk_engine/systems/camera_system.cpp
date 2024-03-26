//
// Created by Ricardo on 26/03/2024.
//

#include <duk_engine/engine.h>
#include <duk_engine/systems/camera_system.h>

#include <duk_renderer/components/camera.h>

namespace duk::engine {

void CameraUpdateSystem::enter(duk::scene::Objects& objects, Engine& engine) {
    auto renderer = engine.renderer();
    duk::renderer::update_cameras(objects, renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::update(duk::scene::Objects& objects, Engine& engine) {
    auto renderer = engine.renderer();
    duk::renderer::update_cameras(objects, renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::exit(duk::scene::Objects& objects, Engine& engine) {
}

}// namespace duk::engine