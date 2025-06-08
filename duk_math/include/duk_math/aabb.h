//
// Created by Ricardo on 31/05/2025.
//

#ifndef DUK_MATH_AABB_H
#define DUK_MATH_AABB_H

#include <duk_math/math.h>

namespace duk::math {

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB();

    void extend(const glm::vec3& point) noexcept;

    void extend(const AABB& aabb) noexcept;

    bool contains(const glm::vec3& point) const noexcept;

    bool intersects(const AABB& aabb) const noexcept;

    bool valid() const noexcept;
};

AABB transform(const glm::mat4& matrix, const AABB& aabb) noexcept;

}

#endif //DUK_MATH_AABB_H
