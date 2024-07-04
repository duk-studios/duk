//
// Created by Ricardo on 26/03/2024.
//

#include <duk_engine/engine.h>
#include <duk_engine/systems/camera_system.h>
#include <duk_renderer/components/camera.h>

namespace duk::engine {
CameraUpdateSystem::CameraUpdateSystem()
    : System(kMainThreadGroup) {
}

void CameraUpdateSystem::enter(duk::objects::Objects& objects, Engine& engine) {
    auto renderer = engine.globals()->get<duk::renderer::Renderer>();
    duk::renderer::update_cameras(objects, renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::update(duk::objects::Objects& objects, Engine& engine) {
    auto renderer = engine.globals()->get<duk::renderer::Renderer>();
    duk::renderer::update_cameras(objects, renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::exit(duk::objects::Objects& objects, Engine& engine) {
}

}// namespace duk::engine