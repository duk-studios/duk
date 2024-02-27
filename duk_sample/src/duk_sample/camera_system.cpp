//
// Created by rov on 11/18/2023.
//

#include <duk_engine/engine.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <duk_sample/camera_system.h>
#include <duk_engine/engine.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>
#include <duk_platform/system.h>


namespace duk::sample {

namespace detail {

static glm::vec3 input_rotation_direction(const duk::engine::Input* input) {
    glm::vec3 direction(0);
    if (input->mouse(duk::platform::MouseButton::LEFT)) {
        direction = glm::vec3(-input->delta_mouse().y, -input->delta_mouse().x, 0);
    } else {
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

void CameraSystem::enter(engine::Engine& engine) {
}

void CameraSystem::update(engine::Engine& engine) {
    auto& objects = engine.director()->scene()->objects();

    auto object = objects.first_with<CameraController>();

    if (!object) {
        return;
    }

    auto input = engine.input();

    const auto deltaTime = engine.timer()->duration().count();

    auto controller = object.component<CameraController>();

    auto rotation = object.component<duk::renderer::Rotation3D>();
    glm::quat rotationValue(glm::vec3(0));
    if (rotation) {
        auto rotationDirection = detail::input_rotation_direction(input);
        rotation->value = glm::normalize(rotation->value * glm::quat(glm::radians(rotationDirection * controller->rotationSpeed * deltaTime)));
        rotationValue = rotation->value;
    }

    auto position = controller.object().component<duk::renderer::Position3D>();
    if (position) {
        auto moveDirection = detail::input_move_direction(input);
        moveDirection = rotationValue * moveDirection;
        position->value += moveDirection * controller->speed * deltaTime;
    }
}

void CameraSystem::exit(engine::Engine& engine) {
}

}// namespace duk::sample
