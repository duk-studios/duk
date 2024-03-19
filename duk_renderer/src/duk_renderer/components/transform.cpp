/// 20/08/2023
/// transform.cpp

#include <duk_renderer/components/transform.h>
#include <duk_scene/objects.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace duk::renderer {

namespace detail {

static glm::mat4 calculate_model(const duk::scene::Object& object) {
    glm::mat4 model(1);

    if (auto position3D = object.component<Position3D>()) {
        model = glm::translate(model, position3D->value);
    }

    if (auto rotation3D = object.component<Rotation3D>()) {
        model *= glm::mat4_cast(rotation3D->value);
    }

    if (auto scale3D = object.component<Scale3D>()) {
        model = glm::scale(model, scale3D->value);
    }

    return model;
}

static void update_transform(duk::scene::Component<Transform> transform) {
    transform->model = calculate_model(transform.object());
    transform->invModel = glm::inverse(transform->model);
}

static void update_transforms(duk::scene::Objects& objects) {
    for (auto object: objects.all_with<Transform>()) {
        update_transform(object);
    }
}

}// namespace detail

glm::vec3 forward(const Transform& transform) {
    return glm::vec3(transform.model * glm::vec4(0, 0, -1, 1));
}

void TransformUpdateSystem::enter(duk::scene::Objects& objects, duk::scene::Environment* environment) {
    detail::update_transforms(objects);
}

void TransformUpdateSystem::update(duk::scene::Objects& objects, duk::scene::Environment* environment) {
    detail::update_transforms(objects);
}

void TransformUpdateSystem::exit(duk::scene::Objects& objects, duk::scene::Environment* environment) {
}

}// namespace duk::renderer