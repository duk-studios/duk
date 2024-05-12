/// 20/08/2023
/// transform.cpp

#include <duk_renderer/components/transform.h>
#include <duk_renderer/material/material.h>

#include <glm/gtc/matrix_transform.hpp>

namespace duk::renderer {

namespace detail {

static glm::mat4 calculate_model(const Transform& transform) {
    glm::mat4 model(1);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(transform.rotation);
    model = glm::scale(model, transform.scale);
    return model;
}

}// namespace detail

glm::vec3 forward(const Transform& transform) {
    return glm::vec3(transform.model * glm::vec4(0, 0, -1, 1));
}

void update_transform(const duk::objects::Component<Transform>& transform) {
    transform->model = detail::calculate_model(*transform);
    transform->invModel = glm::inverse(transform->model);
}

void update_transforms(duk::objects::Objects& objects) {
    for (auto object: objects.all_with<Transform>()) {
        auto transform = object.component<Transform>();

        update_transform(transform);

        if (auto material = find_material(object)) {
            material->set(object.id(), "uTransform", "model", transform->model);
            material->set(object.id(), "uTransform", "invModel", transform->invModel);
        }
    }
}

}// namespace duk::renderer