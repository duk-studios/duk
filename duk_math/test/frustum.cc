//
// Created by rov on 25/07/2026.
//

#include <duk_math/frustum.h>
#include <catch2/catch_test_macros.hpp>

namespace {

duk::math::Frustum make_test_frustum() {
    const glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
    return duk::math::Frustum(view, proj);
}

}// namespace

TEST_CASE("Frustum basic functionality", "[frustum]") {
    const duk::math::Frustum frustum = make_test_frustum();

    SECTION("point in front of the camera and within bounds is inside") {
        CHECK(frustum.intersects(glm::vec3(0.0f, 0.0f, -5.0f)));
    }

    SECTION("point behind the camera is outside") {
        CHECK_FALSE(frustum.intersects(glm::vec3(0.0f, 0.0f, 5.0f)));
    }

    SECTION("point beyond the far plane is outside") {
        CHECK_FALSE(frustum.intersects(glm::vec3(0.0f, 0.0f, -1000.0f)));
    }

    SECTION("point far to the side of the frustum is outside") {
        CHECK_FALSE(frustum.intersects(glm::vec3(1000.0f, 0.0f, -5.0f)));
    }

    SECTION("AABB fully inside the frustum intersects") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(-0.5f, -0.5f, -5.5f));
        aabb.extend(glm::vec3(0.5f, 0.5f, -4.5f));
        CHECK(frustum.intersects(aabb));
    }

    SECTION("AABB far outside the frustum does not intersect") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(1000.0f, 1000.0f, -5.0f));
        aabb.extend(glm::vec3(1001.0f, 1001.0f, -5.0f));
        CHECK_FALSE(frustum.intersects(aabb));
    }

    SECTION("AABB behind the camera does not intersect") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(-0.5f, -0.5f, 5.0f));
        aabb.extend(glm::vec3(0.5f, 0.5f, 6.0f));
        CHECK_FALSE(frustum.intersects(aabb));
    }
}
