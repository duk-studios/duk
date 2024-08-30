//
// Created by Ricardo on 06/04/2024.
//

#ifndef DUK_UI_CANVAS_H
#define DUK_UI_CANVAS_H

#include <duk_objects/objects.h>

#include <duk_renderer/material/uniform_buffer.h>
#include <duk_renderer/material/globals/camera_types.h>

namespace duk::ui {

struct Canvas {
    duk::renderer::globals::ViewProjUBO viewProjUBO;
    uint64_t hash = 0;
};

struct CanvasTransform {
    // normalized position in parent space
    glm::vec2 anchorMin;

    glm::vec2 anchorMax;

    // pixel offset from anchor points
    glm::vec2 anchorMinOffsets;

    glm::vec2 anchorMaxOffsets;

    glm::vec2 pivot;

    // normalized
    glm::vec2 scale;

    // runtime properties
    uint64_t hash = 0;

    glm::vec2 localPosition;

    glm::vec2 localMin;

    glm::vec2 localMax;

    glm::vec2 size;
};

}// namespace duk::ui

namespace duk::serial {

template<>
inline void from_json<duk::ui::CanvasTransform>(const rapidjson::Value& json, duk::ui::CanvasTransform& canvasTransform) {
    from_json_member(json, "anchorMin", canvasTransform.anchorMin);
    from_json_member(json, "anchorMax", canvasTransform.anchorMax);
    from_json_member(json, "anchorMinOffsets", canvasTransform.anchorMinOffsets);
    from_json_member(json, "anchorMaxOffsets", canvasTransform.anchorMaxOffsets);
    from_json_member(json, "pivot", canvasTransform.pivot);
    from_json_member(json, "scale", canvasTransform.scale);
}

template<>
inline void to_json<duk::ui::CanvasTransform>(rapidjson::Document& document, rapidjson::Value& json, const duk::ui::CanvasTransform& canvasTransform) {
    to_json_member(document, json, "anchorMin", canvasTransform.anchorMin);
    to_json_member(document, json, "anchorMax", canvasTransform.anchorMax);
    to_json_member(document, json, "anchorMinOffsets", canvasTransform.anchorMinOffsets);
    to_json_member(document, json, "anchorMaxOffsets", canvasTransform.anchorMaxOffsets);
    to_json_member(document, json, "pivot", canvasTransform.pivot);
    to_json_member(document, json, "scale", canvasTransform.scale);
}

}// namespace duk::serial

#endif//DUK_UI_CANVAS_H
