//
// Created by rov on 25/07/2026.
//

#ifndef DUK_MATH_FRUSTRUM_H
#define DUK_MATH_FRUSTRUM_H

#include <duk_math/aabb.h>

namespace duk::math {

struct Frustum {
    Frustum(const glm::mat4& view, const glm::mat4& proj);

    bool intersects(const glm::vec3& point) const noexcept;

    bool intersects(const AABB& aabb) const noexcept;

    // each plane is stored as (normal.x, normal.y, normal.z, distance), with the normal pointing inward
    glm::vec4 left;
    glm::vec4 right;
    glm::vec4 bottom;
    glm::vec4 top;
    glm::vec4 near;
    glm::vec4 far;
};

}

#endif //DUK_MATH_FRUSTRUM_H
