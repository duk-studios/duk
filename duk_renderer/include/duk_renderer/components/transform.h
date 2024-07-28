/// 20/08/2023
/// transform.h

#ifndef DUK_RENDERER_TRANSFORM_H
#define DUK_RENDERER_TRANSFORM_H

#include <duk_objects/objects.h>

#include <duk_math/math.h>

namespace duk::renderer {

struct Transform {
    glm::vec3 position{0};
    glm::quat rotation{glm::vec3(0)};
    glm::vec3 scale{1};
};

struct Matrices {
    glm::mat4 model{1};
    glm::mat4 invModel{1};
    size_t hash{0};
};

glm::vec3 forward(const duk::objects::Component<Transform>& transform);

glm::vec3 world_position(const duk::objects::Component<Transform>& transform);

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json<duk::renderer::Transform>(const rapidjson::Value& json, duk::renderer::Transform& transform) {
    from_json_member(json, "rotation", transform.rotation);
    from_json_member(json, "position", transform.position);
    from_json_member(json, "scale", transform.scale);
}

template<>
inline void to_json<duk::renderer::Transform>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::Transform& transform) {
    to_json_member(document, json, "position", transform.position);
    to_json_member(document, json, "rotation", transform.rotation);
    to_json_member(document, json, "scale", transform.scale);
}

}// namespace duk::serial

#endif// DUK_RENDERER_TRANSFORM_H
