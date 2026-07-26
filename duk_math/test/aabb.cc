//
// Created by rov on 25/07/2026.
//

#include <duk_math/aabb.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AABB basic functionality", "[aabb]") {
    SECTION("default constructed AABB is invalid") {
        duk::math::AABB aabb;
        CHECK_FALSE(aabb.valid());
    }

    SECTION("extend with point makes it valid and grows bounds") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(1.0f, 2.0f, 3.0f));
        CHECK(aabb.valid());
        CHECK(aabb.min == glm::vec3(1.0f, 2.0f, 3.0f));
        CHECK(aabb.max == glm::vec3(1.0f, 2.0f, 3.0f));

        aabb.extend(glm::vec3(-1.0f, 5.0f, 0.0f));
        CHECK(aabb.min == glm::vec3(-1.0f, 2.0f, 0.0f));
        CHECK(aabb.max == glm::vec3(1.0f, 5.0f, 3.0f));
    }

    SECTION("extend with another AABB grows bounds") {
        duk::math::AABB a;
        a.extend(glm::vec3(0.0f));
        a.extend(glm::vec3(1.0f));

        duk::math::AABB b;
        b.extend(glm::vec3(-2.0f));
        b.extend(glm::vec3(0.5f));

        a.extend(b);
        CHECK(a.min == glm::vec3(-2.0f));
        CHECK(a.max == glm::vec3(1.0f));
    }

    SECTION("contains") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(-1.0f));
        aabb.extend(glm::vec3(1.0f));

        CHECK(aabb.contains(glm::vec3(0.0f)));
        CHECK(aabb.contains(glm::vec3(1.0f)));
        CHECK(aabb.contains(glm::vec3(-1.0f)));
        CHECK_FALSE(aabb.contains(glm::vec3(2.0f, 0.0f, 0.0f)));
    }

    SECTION("intersects") {
        duk::math::AABB a;
        a.extend(glm::vec3(-1.0f));
        a.extend(glm::vec3(1.0f));

        duk::math::AABB overlapping;
        overlapping.extend(glm::vec3(0.5f));
        overlapping.extend(glm::vec3(2.0f));
        CHECK(a.intersects(overlapping));

        duk::math::AABB disjoint;
        disjoint.extend(glm::vec3(5.0f));
        disjoint.extend(glm::vec3(6.0f));
        CHECK_FALSE(a.intersects(disjoint));
    }

    SECTION("transform") {
        duk::math::AABB aabb;
        aabb.extend(glm::vec3(-1.0f));
        aabb.extend(glm::vec3(1.0f));

        const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
        const duk::math::AABB transformed = duk::math::transform(translation, aabb);

        CHECK(transformed.min == glm::vec3(1.0f, -1.0f, -1.0f));
        CHECK(transformed.max == glm::vec3(3.0f, 1.0f, 1.0f));
    }
}
