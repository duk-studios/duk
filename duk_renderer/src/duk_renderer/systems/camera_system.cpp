//
// Created by Ricardo on 26/03/2024.
//

#include <duk_renderer/systems/camera_system.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/camera.h>

namespace duk::renderer {

void CameraUpdateSystem::attach() {
    listen_component<CameraEnterEvent>(this);
}

void CameraUpdateSystem::update() {
    auto renderer = global<duk::renderer::Renderer>();
    duk::renderer::update_cameras(*objects(), renderer->render_width(), renderer->render_height());
}

void CameraUpdateSystem::receive(const CameraEnterEvent& event) {
    auto camera = event.component;
    auto perspectiveCamera = camera.component<duk::renderer::PerspectiveCamera>();
    auto renderer = global<duk::renderer::Renderer>();
    duk::renderer::update_camera(camera, perspectiveCamera, renderer->render_width(), renderer->render_height());
}
}// namespace duk::renderer