//
// Created by rov on 11/18/2023.
//

#include <duk_sample/camera_system.h>
#include <duk_engine/engine.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>
#include <duk_log/log.h>


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

void CameraSystem::init() {

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

    auto input = duk::engine::System::engine()->input();

    auto [position, rotation, pivotRotator] = m_camera.components<duk::renderer::Position3D, duk::renderer::Rotation3D, detail::PivotRotator>();

    const auto time = engine()->timer()->total_duration().count();

    const auto speed = glm::radians(pivotRotator->angularSpeed);

    glm::vec3 offset = {
            cosf((float)time * speed) * pivotRotator->distanceFromPivot,
            5,
            sinf((float)time * speed) * pivotRotator->distanceFromPivot
    };

    auto inputOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    auto movementSpeed = 0.2f;
    auto camSensitivity = 1.0f;
    auto camRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    auto camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    auto camDirection = glm::normalize(position->value - camTarget);
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto cameraRight = glm::normalize(glm::cross(up, camDirection));
    auto cameraUp = glm::cross(camDirection, cameraRight);
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
    auto yaw = -90.0f;
    auto pitch = 0.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


    if(input->key_down(duk::platform::Keys::W)) {
        inputOffset += glm::vec3(0.0f, 0.0f, -1.0f);
    }

    if(input->key_down(duk::platform::Keys::A)) {
        inputOffset += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if(input->key_down(duk::platform::Keys::S)) {
        inputOffset += glm::vec3(0.0f, 0.0f, 1.0f);
    }

    if(input->key_down(duk::platform::Keys::D)) {
        inputOffset += glm::vec3(1.0f, 0.0f, 0.0f);
    }

    if(input->key_down(duk::platform::Keys::Q)) {
        inputOffset += glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if(input->key_down(duk::platform::Keys::E)) {
        inputOffset += glm::vec3(0.0f, -1.0f, 0.0f);
    }

    position->value += inputOffset * movementSpeed * time;

    camRotation.x += input->mouse_x() * camSensitivity;
    camRotation.y += input->mouse_y() * camSensitivity;

    rotation->value = glm::quat(camRotation);
}

}

