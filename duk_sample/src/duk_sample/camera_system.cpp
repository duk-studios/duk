//
// Created by rov on 11/18/2023.
//

#include <duk_sample/camera_system.h>
#include <duk_engine/engine.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>
#include <duk_platform/system.h>


namespace duk::sample {

namespace detail {

static void update_perspective(duk::scene::Component<duk::renderer::PerspectiveCamera> perspectiveCamera, uint32_t width, uint32_t height) {
    perspectiveCamera->zNear = 0.1f;
    perspectiveCamera->zFar = 1000.f;
    perspectiveCamera->aspectRatio = (float)width / (float)height;
    perspectiveCamera->fovDegrees = 45.0f;
}

struct CameraController {
    float speed;
    float rotationSpeed;
};

}// namespace detail

void CameraSystem::enter(engine::Engine& engine) {
    auto window = engine.window();
    auto scene = engine.scene();
    auto renderer = engine.renderer();

    m_listener.listen(window->window_resize_event, [this](uint32_t width, uint32_t height) {
        detail::update_perspective(m_camera.component<duk::renderer::PerspectiveCamera>(), width, height);
    });

    m_camera = scene->objects().add_object();

    detail::update_perspective(m_camera.add<duk::renderer::PerspectiveCamera>(), renderer->render_width(), renderer->render_height());

    auto position = m_camera.add<duk::renderer::Position3D>();
    position->value = glm::vec3(0, 0, 20);
    m_camera.add<duk::renderer::Rotation3D>();
    auto settings = m_camera.add<detail::CameraController>();
    settings->speed = 10.0f;
    settings->rotationSpeed = 50.0f;

    renderer->use_as_camera(m_camera);

    duk::platform::System::create();
}

void CameraSystem::update(engine::Engine& engine) {
    auto input = engine.input();

    const auto deltaTime = engine.timer()->duration().count();

    auto [position, rotation, controller, perspective] = m_camera.components<duk::renderer::Position3D, duk::renderer::Rotation3D, detail::CameraController, duk::renderer::PerspectiveCamera>();


    auto cursor = duk::platform::System::instance()->cursor();
    cursor->show(true);

    glm::vec3 rotationDir = glm::vec3(0);
    if (input->mouse(duk::platform::MouseButton::LEFT)) {
        rotationDir = glm::vec3(-input->delta_mouse().y, -input->delta_mouse().x, 0);
        cursor->set_cursor(duk::platform::CursorType::DRAG);
    } else {
        cursor->set_cursor(duk::platform::CursorType::ARROW);
        if (input->key(duk::platform::Keys::UP_ARROW)) {
            rotationDir = glm::vec3(-1, 0, 0);
        }
        if (input->key(duk::platform::Keys::DOWN_ARROW)) {
            rotationDir = glm::vec3(1, 0, 0);
        }
        if (input->key(duk::platform::Keys::RIGHT_ARROW)) {
            rotationDir = glm::vec3(0, -1, 0);
        }
        if (input->key(duk::platform::Keys::LEFT_ARROW)) {
            rotationDir = glm::vec3(0, 1, 0);
        }
    }

    auto inputOffset = glm::vec3(0.0f, 0.0f, 0.0f);

    rotation->value = glm::normalize(rotation->value * glm::quat(glm::radians(rotationDir * controller->rotationSpeed * deltaTime)));

    if (input->key(duk::platform::Keys::W)) {
        inputOffset += glm::vec3(0.0f, 0.0f, -1.0f);
    }

    if (input->key(duk::platform::Keys::A)) {
        inputOffset += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if (input->key(duk::platform::Keys::S)) {
        inputOffset += glm::vec3(0.0f, 0.0f, 1.0f);
    }

    if (input->key(duk::platform::Keys::D)) {
        inputOffset += glm::vec3(1.0f, 0.0f, 0.0f);
    }

    if (input->key(duk::platform::Keys::Q)) {
        inputOffset += glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if (input->key(duk::platform::Keys::E)) {
        inputOffset += glm::vec3(0.0f, -1.0f, 0.0f);
    }

    inputOffset = rotation->value * inputOffset;

    position->value += inputOffset * controller->speed * deltaTime;
}

void CameraSystem::exit(engine::Engine& engine) {
}
}// namespace duk::sample
