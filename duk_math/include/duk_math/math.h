//
// Created by sidao on 30/06/2024.
//

#ifndef DUK_MATH_MATH_H
#define DUK_MATH_MATH_H

#include <random>
#include <glm/gtc/random.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/scalar_constants.hpp>

namespace duk::math {

float generate_random_float(float min, float max, float scaleFactor = 1.0f);

int generate_random_int(int min, int max, int scaleFactor = 1.0f);

glm::vec3 smooth_damp(const glm::vec3& current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

}// namespace duk::math
#endif//DUK_MATH_MATH_H
