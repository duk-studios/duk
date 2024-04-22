//
// Created by rov on 11/18/2023.
//

#include <duk_engine/engine.h>
#include <duk_platform/systems.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <duk_sample/camera_system.h>

namespace duk::sample {

namespace detail {

static glm::vec3 input_rotation_direction(const duk::engine::Input* input, duk::platform::Cursor* cursor) {
    glm::vec3 direction(0);
    if (input->mouse(duk::platform::MouseButton::LEFT)) {
        cursor->set_cursor(duk::platform::CursorType::DRAG);
        direction = glm::vec3(-input->delta_mouse().y, -input->delta_mouse().x, 0);
    } else {
        cursor->set_cursor(duk::platform::CursorType::ARROW);
        if (input->key(duk::platform::Keys::UP_ARROW)) {
            direction = glm::vec3(-1, 0, 0);
        }
        if (input->key(duk::platform::Keys::DOWN_ARROW)) {
            direction = glm::vec3(1, 0, 0);
        }
        if (input->key(duk::platform::Keys::RIGHT_ARROW)) {
            direction = glm::vec3(0, -1, 0);
        }
        if (input->key(duk::platform::Keys::LEFT_ARROW)) {
            direction = glm::vec3(0, 1, 0);
        }
    }
    return direction;
}

static glm::vec3 input_move_direction(const duk::engine::Input* input) {
    glm::vec3 direction(0);
    if (input->key(duk::platform::Keys::W)) {
        direction += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (input->key(duk::platform::Keys::A)) {
        direction += glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    if (input->key(duk::platform::Keys::S)) {
        direction += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (input->key(duk::platform::Keys::D)) {
        direction += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (input->key(duk::platform::Keys::Q)) {
        direction += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (input->key(duk::platform::Keys::E)) {
        direction += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    return direction;
}

}// namespace detail

CameraSystem::CameraSystem()
    : System(kMainThreadGroup) {
}

void CameraSystem::enter(duk::objects::Objects& objects, duk::engine::Engine& engine) {
}

void CameraSystem::update(duk::objects::Objects& objects, duk::engine::Engine& engine) {
    auto object = objects.first_with<CameraController>();

    if (!object) {
        return;
    }

    auto input = engine.input();

    const auto deltaTime = engine.timer()->duration().count();
    auto cursor = duk::platform::System::instance()->cursor();

    auto controller = object.component<CameraController>();

    auto transform = object.component<duk::renderer::Transform>();
    if (!transform) {
        duk::log::fatal("No transform attached to CameraController, skipping CameraSystem");
        return;
    }
    auto rotationDirection = detail::input_rotation_direction(input, cursor);
    transform->rotation = glm::normalize(transform->rotation * glm::quat(glm::radians(rotationDirection * controller->rotationSpeed * deltaTime)));

    auto moveDirection = detail::input_move_direction(input);
    moveDirection = transform->rotation * moveDirection;
    transform->position += moveDirection * controller->speed * deltaTime;

    if (input->mouse_down(duk::platform::MouseButton::RIGHT)) {
        auto camera = object.component<duk::renderer::Camera>();
        auto worldPos = duk::renderer::screen_to_world(camera, engine.window()->size(), glm::vec3(input->mouse_position(), -30));

        auto sphereTransform = objects.copy_objects(*controller->sphere).component<duk::renderer::Transform>();
        sphereTransform->position = worldPos;

        controller->audioPlayer.play(engine.audio(), controller->spawnClip.get());
    }
}

void CameraSystem::exit(duk::objects::Objects& objects, duk::engine::Engine& engine) {
}

}// namespace duk::sample
