//
// Created by sidao on 30/06/2024.
//

#include <duk_math/math.h>

namespace duk::math {

float generate_random_float(float min, float max, float scaleFactor) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rand_number(min * scaleFactor, max * scaleFactor);
    return rand_number(gen);
}

float generate_random_inte(int min, int max, int scaleFactor) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> rand_number(min * scaleFactor, max * scaleFactor);
    return rand_number(gen);
}

glm::vec3 smooth_damp(const glm::vec3& current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
    float outputX = 0.0f;
    float outputY = 0.0f;
    float outputZ = 0.0f;

    smoothTime = glm::max(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float changeX = current.x - target.x;
    float changeY = current.y - target.y;
    float changeZ = current.z - target.z;
    glm::vec3 originalTo = target;

    if (abs(changeX) <= 0.1f && abs(changeY) <= 0.1f && abs(changeZ) <= 0.1f) {
        return {current.x, current.y, current.z};
    }

    float maxChange = maxSpeed * smoothTime;
    float maxChangeSq = maxChange * maxChange;
    float sqrMag = changeX * changeX + changeY * changeY + changeZ * changeZ;

    if (sqrMag > maxChangeSq) {
        auto mag = glm::sqrt(sqrMag);
        changeX = changeX / mag * maxChange;
        changeY = changeY / mag * maxChange;
        changeZ = changeZ / mag * maxChange;
    }

    target.x = current.x - changeX;
    target.y = current.y - changeY;
    target.z = current.z - changeZ;

    auto tempX = (currentVelocity.x + omega * changeX) * deltaTime;
    auto tempY = (currentVelocity.y + omega * changeY) * deltaTime;
    auto tempZ = (currentVelocity.z + omega * changeZ) * deltaTime;

    currentVelocity.x = (currentVelocity.x - omega * tempX) * exp;
    currentVelocity.y = (currentVelocity.y - omega * tempY) * exp;
    currentVelocity.z = (currentVelocity.z - omega * tempZ) * exp;

    outputX = target.x + (changeX + tempX) * exp;
    outputY = target.y + (changeY + tempY) * exp;
    outputZ = target.z + (changeZ + tempZ) * exp;

    float origMinusCurrentX = originalTo.x - current.x;
    float origMinusCurrentY = originalTo.y - current.y;
    float origMinusCurrentZ = originalTo.z - current.z;
    float outMinusOrigX = outputX - originalTo.x;
    float outMinusOrigY = outputY - originalTo.y;
    float outMinusOrigZ = outputZ - originalTo.z;

    if (origMinusCurrentX * outMinusOrigX + origMinusCurrentY * outMinusOrigY + origMinusCurrentZ * outMinusOrigZ > 0) {
        outputX = originalTo.x;
        outputY = originalTo.y;
        outputZ = originalTo.z;

        currentVelocity.x = (outputX - originalTo.x) / deltaTime;
        currentVelocity.y = (outputY - originalTo.y) / deltaTime;
        currentVelocity.z = (outputZ - originalTo.z) / deltaTime;
    }

    return {outputX, outputY, outputZ};
}
}// namespace duk::math
