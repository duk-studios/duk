/// 20/08/2023
/// transform.cpp

#include <duk_renderer/components/transform.h>
#include <duk_scene/objects.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace duk::renderer {

glm::mat4 model_matrix_3d(const duk::scene::Object& object) {
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

glm::mat3 model_matrix_2d(const duk::scene::Object& object) {
    glm::mat3 model(1);

    if (auto position2D = object.component<Position2D>()) {
        model = glm::translate(model, position2D->value);
    }

    if (auto rotation2D = object.component<Rotation2D>()) {
        model = glm::rotate(model, rotation2D->value);
    }

    if (auto scale2D = object.component<Scale2D>()) {
        model = glm::scale(model, scale2D->value);
    }

    return model;
}

glm::vec3 forward_direction_3d(const scene::Object& object) {
    return glm::vec3(model_matrix_3d(object) * glm::vec4(0, 0, 1, 0));
}

}// namespace duk::renderer