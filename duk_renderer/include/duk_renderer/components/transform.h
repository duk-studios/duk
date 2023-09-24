/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_scene/object.h>

#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

namespace duk::renderer {

template<typename T>
struct Position {
    T value;
};

using Position2D = Position<glm::vec2>;
using Position3D = Position<glm::vec3>;

template<typename T>
struct Rotation {
    T value;
};

using Rotation2D = Rotation<float>;
using Rotation3D = Rotation<glm::quat>;


template<typename T>
struct Scale {
    T value {1};
};

using Scale2D = Scale<glm::vec2>;
using Scale3D = Scale<glm::vec3>;

glm::mat4 model_matrix_3d(const duk::scene::Object& object);

glm::mat3 model_matrix_2d(const duk::scene::Object& object);

}

#endif // DUK_RENDERER_TRANSFORM_H

