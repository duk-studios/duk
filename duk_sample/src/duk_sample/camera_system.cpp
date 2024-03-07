//
// Created by rov on 11/18/2023.
//

#include <duk_engine/engine.h>
#include <duk_platform/system.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/pools/material_pool.h>
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

void CameraSystem::enter(duk::scene::Objects& objects, duk::scene::Environment* environment) {
}

void CameraSystem::update(duk::scene::Objects& objects, duk::scene::Environment* environment) {
    auto engine = environment->as<duk::engine::Engine>();

    auto object = objects.first_with<CameraController>();

    if (!object) {
        return;
    }

    auto input = engine->input();

    const auto deltaTime = engine->timer()->duration().count();
    auto cursor = duk::platform::System::instance()->cursor();

    auto controller = object.component<CameraController>();

    auto rotation = object.component<duk::renderer::Rotation3D>();
    glm::quat rotationValue(glm::vec3(0));
    if (rotation) {
        auto rotationDirection = detail::input_rotation_direction(input, cursor);
        rotation->value = glm::normalize(rotation->value * glm::quat(glm::radians(rotationDirection * controller->rotationSpeed * deltaTime)));
        rotationValue = rotation->value;
    }

    auto position = controller.object().component<duk::renderer::Position3D>();
    if (position) {
        auto moveDirection = detail::input_move_direction(input);
        moveDirection = rotationValue * moveDirection;
        position->value += moveDirection * controller->speed * deltaTime;
    }

    if (input->mouse_down(duk::platform::MouseButton::RIGHT)) {
        auto camera = object.component<duk::renderer::Camera>();
        auto worldPos = camera->screen_to_world(engine->window()->size(), glm::vec3(input->mouse_position(), -30));
        duk::log::debug("World pos: {0},{1},{2}", worldPos.x, worldPos.y, worldPos.z);

        auto newObject = objects.add_object();

        auto position = newObject.add<duk::renderer::Position3D>();
        position->value = worldPos;

        auto meshRenderer = newObject.add<duk::renderer::MeshRenderer>();
        meshRenderer->mesh = engine->pools()->get<duk::renderer::MeshPool>()->sphere();
        meshRenderer->material = engine->pools()->get<duk::renderer::MaterialPool>()->find(duk::resource::Id(1000013));
    }
}

void CameraSystem::exit(duk::scene::Objects& objects, duk::scene::Environment* environment) {
}

}// namespace duk::sample
