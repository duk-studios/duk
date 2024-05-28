//
// Created by Ricardo on 06/04/2024.
//

#ifndef DUK_RENDERER_CANVAS_H
#define DUK_RENDERER_CANVAS_H

#include <duk_objects/objects.h>

#include <glm/glm.hpp>

namespace duk::renderer {

struct Canvas {
    glm::ivec2 size;
    glm::mat4 proj;
};

struct CanvasTransform {
    // normalized
    glm::vec2 anchor;

    // normalized
    glm::vec2 pivot;

    // in pixels
    glm::vec2 position;

    // in pixels
    glm::vec2 size;

    // normalized
    glm::vec2 scale;

    // internal usage
    glm::mat4 model;
};

void update_canvas(const duk::objects::Component<Canvas>& canvas, uint32_t width, uint32_t height);

void update_canvas_transform(const duk::objects::Component<Canvas>& canvas, duk::objects::Component<CanvasTransform>& canvasTransform);

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json<duk::renderer::Canvas>(const rapidjson::Value& json, duk::renderer::Canvas& canvas) {
}

template<>
inline void to_json<duk::renderer::Canvas>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::Canvas& canvas) {
}

template<>
inline void from_json<duk::renderer::CanvasTransform>(const rapidjson::Value& json, duk::renderer::CanvasTransform& canvasTransform) {
    from_json_member(json, "anchor", canvasTransform.anchor);
    from_json_member(json, "pivot", canvasTransform.pivot);
    from_json_member(json, "position", canvasTransform.position);
    from_json_member(json, "size", canvasTransform.size);
    from_json_member(json, "scale", canvasTransform.scale);
}

template<>
inline void to_json<duk::renderer::CanvasTransform>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::CanvasTransform& canvasTransform) {
    to_json_member(document, json, "anchor", canvasTransform.anchor);
    to_json_member(document, json, "pivot", canvasTransform.pivot);
    to_json_member(document, json, "position", canvasTransform.position);
    to_json_member(document, json, "size", canvasTransform.size);
    to_json_member(document, json, "scale", canvasTransform.scale);
}

}// namespace duk::serial

#endif//DUK_RENDERER_CANVAS_H
