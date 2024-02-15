//
// Created by rov on 11/18/2023.
//

#include <duk_sample/camera_system.h>
#include <duk_engine/engine.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>


namespace duk::sample {

namespace detail {

static void update_perspective(duk::scene::Component<duk::renderer::PerspectiveCamera> perspectiveCamera, uint32_t width, uint32_t height){
    perspectiveCamera->zNear = 0.1f;
    perspectiveCamera->zFar = 1000.f;
    perspectiveCamera->aspectRatio = (float)width / (float)height;
    perspectiveCamera->fovDegrees = 45.0f;
}

struct CameraSettings {
    float speed;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float sensitivity = 0.1f;
    float lastX, lastY;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
};

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

    auto position = m_camera.add<duk::renderer::Position3D>();
    position->value = glm::vec3(0, 0, 20);
    m_camera.add<duk::renderer::Rotation3D>();
    auto settings = m_camera.add<detail::CameraSettings>();
    settings->speed = 10.0f;
    settings->lastX = window->width() / 2;
    settings->lastY = window->height() / 2;


    renderer->use_as_camera(m_camera);
}

void CameraSystem::update() {

    auto input = duk::engine::System::engine()->input();

    auto [position, rotation, settings, perspective] = m_camera.components<duk::renderer::Position3D, duk::renderer::Rotation3D, detail::CameraSettings,duk::renderer::PerspectiveCamera>();

    const auto deltaTime = engine()->timer()->duration().count();

    auto inputOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    auto camRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    auto camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    auto camDirection = glm::normalize(position->value - camTarget);
    auto worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    settings->right = glm::normalize(glm::cross(worldUp, camDirection));
    settings->up = glm::cross(camDirection, settings->right);

    glm::vec3 direction;
    direction.x = cos(glm::radians(settings->yaw)) * cos(glm::radians(settings->pitch));
    direction.y = sin(glm::radians(settings->pitch));
    direction.z = sin(glm::radians(settings->yaw)) * cos(glm::radians(settings->pitch));
    settings->front = glm::normalize(direction);

    float xOffset = input->mouse_x() - settings->lastX;
    float yOffset = settings->lastY - input->mouse_y();
    settings->lastX = input->mouse_x();
    settings->lastY = input->mouse_y();
    xOffset *= settings->sensitivity;
    yOffset *= settings->sensitivity;
    settings->yaw += xOffset;
    settings->pitch += yOffset;

    if(settings->pitch > 89.0f)
        settings->pitch =  89.0f;
    if(settings->pitch < -89.0f)
        settings->pitch = -89.0f;

    rotation->value = glm::lookAt(position->value, position->value + settings->front, settings->up);

    perspective->fovDegrees -= input->mouse_wheel();

    if (perspective->fovDegrees < 1.0f)
        perspective->fovDegrees = 1.0f;
    if (perspective->fovDegrees > 45.0f)
        perspective->fovDegrees = 45.0f;

    if(input->key(duk::platform::Keys::W)) {
        inputOffset += glm::vec3(0.0f, 0.0f, -1.0f);
    }

    if(input->key(duk::platform::Keys::A)) {
        inputOffset += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if(input->key(duk::platform::Keys::S)) {
        inputOffset += glm::vec3(0.0f, 0.0f, 1.0f);
    }

    if(input->key(duk::platform::Keys::D)) {
        inputOffset += glm::vec3(1.0f, 0.0f, 0.0f);
    }

    if(input->key(duk::platform::Keys::Q)) {
        inputOffset += glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if(input->key(duk::platform::Keys::E)) {
        inputOffset += glm::vec3(0.0f, -1.0f, 0.0f);
    }

    position->value += inputOffset * settings->speed * deltaTime;
}

}

