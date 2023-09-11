/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_scene/object.h>

#include <glm/matrix.hpp>

namespace duk::renderer {

template<typename T>
struct Position {
    T value;
};

using Position2D = Position<glm::vec2>;
using Position3D = Position<glm::vec3>;

glm::mat4 model_matrix_3d(const duk::scene::Object& object);

glm::mat3 model_matrix_2d(const duk::scene::Object& object);

}

#endif // DUK_RENDERER_TRANSFORM_H

