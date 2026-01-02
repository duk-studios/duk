//
// Created by sidao on 30/06/2024.
//

#ifndef DUK_MATH_MATH_H
#define DUK_MATH_MATH_H

#include <duk_math/glm.h>

namespace duk::math {

glm::vec3 smooth_damp(const glm::vec3& current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

}// namespace duk::math
#endif//DUK_MATH_MATH_H
