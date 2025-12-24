//
// Created by rov on 23/12/2025.
//

#ifndef DUK_MATH_GLM_IMPORT_H
#define DUK_MATH_GLM_IMPORT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

// serialization support for glm types
#include <duk_serial/json.h>

#include <sstream>

namespace duk::serial {

template<glm::length_t L, typename T, glm::qualifier Q>
struct JsonPrimitiveValue<glm::vec<L, T, Q>> {

    static void write(rapidjson::Document& document, rapidjson::Value& json, const glm::vec<L, T, Q>& value);

    static void read(const rapidjson::Value& json, glm::vec<L, T, Q>& value);
};

template<typename T, glm::qualifier Q>
struct JsonPrimitiveValue<glm::qua<T, Q>> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const glm::qua<T, Q>& value);

    static void read(const rapidjson::Value& json, glm::qua<T, Q>& value);
};

template<glm::length_t L, typename T, glm::qualifier Q>
void JsonPrimitiveValue<glm::vec<L, T, Q>>::write(rapidjson::Document& document, rapidjson::Value& json, const glm::vec<L, T, Q>& value) {
    if constexpr (L == 1) {
        json.SetString(fmt::format("{0}", value[0]), document.GetAllocator());
    } else if constexpr (L == 2) {
        json.SetString(fmt::format("{0};{1}", value[0], value[1]), document.GetAllocator());
    } else if constexpr (L == 3) {
        json.SetString(fmt::format("{0};{1};{2}", value[0], value[1], value[2]), document.GetAllocator());
    } else if constexpr (L == 4) {
        json.SetString(fmt::format("{0};{1};{2};{3}", value[0], value[1], value[2], value[3]), document.GetAllocator());
    } else {
        static_assert(false, "Unsupported type");
    }
}

template<glm::length_t L, typename T, glm::qualifier Q>
void JsonPrimitiveValue<glm::vec<L, T, Q>>::read(const rapidjson::Value& json, glm::vec<L, T, Q>& value) {
    std::stringstream str(json.GetString());
    std::string segment;
    for (int i = 0; std::getline(str, segment, ';') && i < L; i++) {
        if constexpr (std::is_same_v<T, float>) {
            value[i] = std::stof(segment);
        } else if constexpr (std::is_same_v<T, double>) {
            value[i] = std::stod(segment);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            value[i] = std::stoi(segment);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            value[i] = std::stoul(segment);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            value[i] = std::stoll(segment);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            value[i] = std::stoull(segment);
        } else if constexpr (std::is_same_v<T, bool>) {
            value[i] = std::stoi(segment) != 0;
        } else {
            static_assert(false, "Unsupported type");
        }
    }
}

template<typename T, glm::qualifier Q>
void JsonPrimitiveValue<glm::qua<T, Q>>::write(rapidjson::Document& document, rapidjson::Value& json, const glm::qua<T, Q>& value) {
    JsonPrimitiveValue<glm::vec<3, T, Q>>::write(document, json, glm::degrees(glm::eulerAngles(value)));
}

template<typename T, glm::qualifier Q>
void JsonPrimitiveValue<glm::qua<T, Q>>::read(const rapidjson::Value& json, glm::qua<T, Q>& value) {
    glm::vec<3, T, Q> vec;
    JsonPrimitiveValue<glm::vec<3, T, Q>>::read(json, vec);
    value = glm::quat(glm::radians(vec));
}

}

#endif //DUK_MATH_GLM_IMPORT_H
