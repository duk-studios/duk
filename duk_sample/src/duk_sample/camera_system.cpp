//
// Created by rov on 11/18/2023.
//

#include <duk_engine/engine.h>
#include <duk_platform/platform.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <duk_sample/camera_system.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <glm/gtc/random.hpp>

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

void CameraSystem::enter(duk::objects::Objects& objects, duk::tools::Globals& globals) {
}

void CameraSystem::update(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    auto object = objects.first_with<CameraController>();

    if (!object) {
        return;
    }

    auto input = globals.get<duk::engine::Input>();
    auto timer = globals.get<duk::tools::Timer>();
    auto audio = globals.get<duk::audio::AudioEngine>();
    auto platform = globals.get<duk::platform::Platform>();
    auto window = globals.get<duk::platform::Window>();

    const auto deltaTime = timer->delta_time();
    auto cursor = platform->cursor();

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
        auto worldPos = duk::renderer::screen_to_world(camera, window->size(), glm::vec3(input->mouse_position(), -30));

        auto spawnedObject = objects.copy_objects(*controller->sphere);
        auto spawnedTransform = spawnedObject.component<duk::renderer::Transform>();
        spawnedTransform->position = worldPos;

        if (auto spriteRenderer = spawnedObject.component<duk::renderer::SpriteRenderer>()) {
            static int spriteIndex = 0;
            spriteRenderer->index = spriteIndex++ % spriteRenderer->sprite->count();
        }

        controller->audioPlayer.play(audio, controller->spawnClip.get(), 1.0f, glm::linearRand(1.0f, 1.05f));
    }
}

void CameraSystem::exit(duk::objects::Objects& objects, duk::tools::Globals& globals) {
}
}// namespace duk::sample
