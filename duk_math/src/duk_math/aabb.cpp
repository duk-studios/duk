//
// Created by Ricardo on 31/05/2025.
//

#include <duk_math/aabb.h>

namespace duk::math {

AABB::AABB()
    : min(std::numeric_limits<float>::max())
    , max(std::numeric_limits<float>::lowest()) {
}

void AABB::extend(const glm::vec3& point) noexcept {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABB::extend(const AABB& aabb) noexcept {
    min = glm::min(min, aabb.min);
    max = glm::max(max, aabb.max);
}

bool AABB::contains(const glm::vec3& point) const noexcept {
    return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z && point.z <= max.z;
}

bool AABB::intersects(const AABB& aabb) const noexcept {
    return min.x <= aabb.max.x && max.x >= aabb.min.x && min.y <= aabb.max.y && max.y >= aabb.min.y && min.z <= aabb.max.z && max.z >= aabb.min.z;
}

bool AABB::valid() const noexcept {
    return min.x <= max.x && min.y <= max.y && min.z <= max.z;
}

AABB transform(const glm::mat4& matrix, const AABB& aabb) noexcept {
    // Calculate the 8 corner points of the AABB
    // clang-format off
    glm::vec3 corners[8] = {
        glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z)
      };
    // clang-format on

    // Apply the transformation to each corner point
    for (int i = 0; i < 8; ++i) {
        corners[i] = glm::vec3(matrix * glm::vec4(corners[i], 1.0f));
    }

    // Recompute the new min and max of the transformed AABB
    AABB transformedAABB;
    for (int i = 0; i < 8; ++i) {
        transformedAABB.extend(corners[i]);
    }
    return transformedAABB;
}

}// namespace duk::math
