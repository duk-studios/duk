//
// Created by rov on 11/18/2023.
//

#include <duk_sample/camera_system.h>
#include <duk_engine/engine.h>
#include <duk_renderer/components/transform.h>
#include "duk_renderer/components/camera.h"


namespace duk::sample {

namespace detail {

struct PivotRotator {
    glm::vec3 pivot;
    float distanceFromPivot;
    float angularSpeed;
};

static void update_perspective(duk::scene::Component<duk::renderer::PerspectiveCamera> perspectiveCamera, uint32_t width, uint32_t height)
{
    perspectiveCamera->zNear = 0.1f;
    perspectiveCamera->zFar = 1000.f;
    perspectiveCamera->aspectRatio = (float)width / (float)height;
    perspectiveCamera->fovDegrees = 45.0f;
}

}

CameraSystem::CameraSystem(duk::engine::Engine &engine) :
    duk::engine::System(engine, "CameraSystem") {

    auto window = duk::engine::System::engine()->window();
    auto scene = duk::engine::System::engine()->scene();
    auto renderer = duk::engine::System::engine()->renderer();

    m_listener.listen(window->window_resize_event, [this](uint32_t width, uint32_t height) {
        detail::update_perspective(m_camera.component<duk::renderer::PerspectiveCamera>(), width, height);
    });

    m_camera = scene->add_object();

    detail::update_perspective(m_camera.add<duk::renderer::PerspectiveCamera>(), renderer->render_width(), renderer->render_height());

    m_camera.add<duk::renderer::Position3D>();
    m_camera.add<duk::renderer::Rotation3D>();

    auto pivotRotator = m_camera.add<detail::PivotRotator>();
    pivotRotator->pivot = glm::vec3(0);
    pivotRotator->distanceFromPivot = 50.f;
    pivotRotator->angularSpeed = 30.f;

    renderer->use_as_camera(m_camera);
}

void CameraSystem::update() {

    auto [position, rotation, pivotRotator] = m_camera.components<duk::renderer::Position3D, duk::renderer::Rotation3D, detail::PivotRotator>();

    const auto time = engine()->timer()->total_duration().count();

    const auto speed = glm::radians(pivotRotator->angularSpeed);

    glm::vec3 offset = {
            cosf((float)time * speed) * pivotRotator->distanceFromPivot,
            5,
            sinf((float)time * speed) * pivotRotator->distanceFromPivot
    };

    position->value = pivotRotator->pivot + offset;

    // calculate rotation
    {
        const auto direction = -glm::normalize(offset);

        rotation->value = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    }}

}

