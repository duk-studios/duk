/// 20/08/2023
/// transform.cpp

#include <duk_objects/objects.h>
#include <duk_renderer/components/transform.h>
#include <glm/gtc/matrix_transform.hpp>

namespace duk::renderer {

namespace detail {

static glm::mat4 calculate_model(const duk::objects::Object& object) {
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

}// namespace detail

glm::vec3 forward(const Transform& transform) {
    return glm::vec3(transform.model * glm::vec4(0, 0, -1, 1));
}

void update_transform(const duk::objects::Component<Transform>& transform) {
    transform->model = detail::calculate_model(transform.object());
    transform->invModel = glm::inverse(transform->model);
}

void update_transforms(duk::objects::Objects& objects) {
    for (auto object: objects.all_with<Transform>()) {
        update_transform(object);
    }
}

}// namespace duk::renderer