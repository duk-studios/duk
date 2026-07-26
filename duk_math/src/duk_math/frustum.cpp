//
// Created by rov on 25/07/2026.
//

#include <duk_math/frustum.h>

namespace duk::math {

namespace detail {

glm::vec4 matrix_row(const glm::mat4& matrix, int row) noexcept {
    return {matrix[0][row], matrix[1][row], matrix[2][row], matrix[3][row]};
}

glm::vec4 normalize_plane(const glm::vec4& plane) noexcept {
    return plane / glm::length(glm::vec3(plane));
}

}// namespace detail

Frustum::Frustum(const glm::mat4& view, const glm::mat4& proj) {
    const glm::mat4 combined = proj * view;

    const glm::vec4 row0 = detail::matrix_row(combined, 0);
    const glm::vec4 row1 = detail::matrix_row(combined, 1);
    const glm::vec4 row2 = detail::matrix_row(combined, 2);
    const glm::vec4 row3 = detail::matrix_row(combined, 3);

    left = detail::normalize_plane(row3 + row0);
    right = detail::normalize_plane(row3 - row0);
    bottom = detail::normalize_plane(row3 + row1);
    top = detail::normalize_plane(row3 - row1);
    // depth range is [0, 1] (GLM_FORCE_DEPTH_ZERO_TO_ONE), so the near plane is row2 directly
    near = detail::normalize_plane(row2);
    far = detail::normalize_plane(row3 - row2);
}

bool Frustum::intersects(const glm::vec3& point) const noexcept {
    const glm::vec4 planes[] = {left, right, bottom, top, near, far};
    for (const auto& plane : planes) {
        if (glm::dot(glm::vec3(plane), point) + plane.w < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Frustum::intersects(const AABB& aabb) const noexcept {
    const glm::vec4 planes[] = {left, right, bottom, top, near, far};
    for (const auto& plane : planes) {
        const glm::vec3 normal(plane);
        // pick the AABB corner furthest along the plane normal, if that one is outside, the whole box is outside
        const glm::vec3 positive(normal.x >= 0.0f ? aabb.max.x : aabb.min.x, normal.y >= 0.0f ? aabb.max.y : aabb.min.y, normal.z >= 0.0f ? aabb.max.z : aabb.min.z);
        if (glm::dot(normal, positive) + plane.w < 0.0f) {
            return false;
        }
    }
    return true;
}

}// namespace duk::math
