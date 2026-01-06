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
    glm::ivec2 size;
    duk::renderer::globals::ViewProjUBO ubo;
    uint64_t hash = 0;
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

}// namespace duk::ui

namespace duk::type {

// clang-format off
template<>
struct Type<duk::ui::Canvas> : Class<duk::ui::Canvas,
    Member<"size", &duk::ui::Canvas::size>> {
};

template<>
struct Type<duk::ui::CanvasTransform> : Class<duk::ui::CanvasTransform,
    Member<"anchor", &duk::ui::CanvasTransform::anchor>,
    Member<"pivot", &duk::ui::CanvasTransform::pivot>,
    Member<"position", &duk::ui::CanvasTransform::position>,
    Member<"size", &duk::ui::CanvasTransform::size>,
    Member<"scale", &duk::ui::CanvasTransform::scale>> {
};

// clang-format on

}// namespace duk::type

#endif//DUK_UI_CANVAS_H
