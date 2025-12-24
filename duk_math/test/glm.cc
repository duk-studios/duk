//
// Created by rov on 10/4/2025.
//

#include <duk_math/glm.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("glm types can be serialized with json", "[json]") {
    SECTION("vec1 roundtrip") {
        glm::vec1 input{3.14f};
        auto json = duk::serial::json_write(input);
        INFO("Serialized JSON:" << json);
        glm::vec1 output = duk::serial::json_read<glm::vec1>(json);
        CHECK(input == output);
    }
    SECTION("vec2 roundtrip") {
        glm::vec2 input{3.14f, 2.71f};
        auto json = duk::serial::json_write(input);
        INFO("Serialized JSON:" << json);
        glm::vec2 output = duk::serial::json_read<glm::vec2>(json);
        CHECK(input == output);
    }
    SECTION("vec3 roundtrip") {
        glm::vec3 input{3.14f, 2.71f, 1.61f};
        auto json = duk::serial::json_write(input);
        INFO("Serialized JSON:" << json);
        glm::vec3 output = duk::serial::json_read<glm::vec3>(json);
        CHECK(input == output);
    }
    SECTION("vec4 roundtrip") {
        glm::vec4 input{3.14f, 2.71f, 1.61f, 0.577f};
        auto json = duk::serial::json_write(input);
        INFO("Serialized JSON:" << json);
        glm::vec4 output = duk::serial::json_read<glm::vec4>(json);
        CHECK(input == output);
    }
    SECTION("quat roundtrip") {
        glm::quat input = glm::quat(glm::radians(glm::vec3{45.0f, 30.0f, 60.0f}));
        auto json = duk::serial::json_write(input);
        INFO("Serialized JSON:" << json);
        glm::quat output = duk::serial::json_read<glm::quat>(json);
        // Due to floating point precision, we check that the difference is small
        CHECK(glm::length(input - output) < 0.0001f);
    }
    SECTION("vec of different types roundtrip") {
        glm::vec<3, int> inputInt{1, 2, 3};
        auto jsonInt = duk::serial::json_write(inputInt);
        INFO("Serialized int JSON:" << jsonInt);
        glm::vec<3, int> outputInt = duk::serial::json_read<glm::vec<3, int>>(jsonInt);
        CHECK(inputInt == outputInt);

        glm::vec<2, double> inputDouble{3.14, 2.71};
        auto jsonDouble = duk::serial::json_write(inputDouble);
        INFO("Serialized double JSON:" << jsonDouble);
        glm::vec<2, double> outputDouble = duk::serial::json_read<glm::vec<2, double>>(jsonDouble);
        CHECK(inputDouble == outputDouble);
    }
}