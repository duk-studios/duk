//
// Created by Ricardo on 06/04/2024.
//

#ifndef DUK_RENDERER_CANVAS_H
#define DUK_RENDERER_CANVAS_H

#include <duk_serial/json_serializer.h>

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

    // internal usage
    glm::mat4 model;
};

void update_canvas(const duk::objects::Component<Canvas>& canvas, uint32_t width, uint32_t height);

void update_canvas_transform(const duk::objects::Component<Canvas>& canvas, const duk::objects::Component<CanvasTransform>& canvasTransform);

}

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::Canvas& canvas) {
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::CanvasTransform& canvasTransform) {
    visitor->visit_member(canvasTransform.anchor, MemberDescription("anchor"));
    visitor->visit_member(canvasTransform.pivot, MemberDescription("pivot"));
    visitor->visit_member(canvasTransform.position, MemberDescription("position"));
    visitor->visit_member(canvasTransform.size, MemberDescription("size"));
}

}

#endif //DUK_RENDERER_CANVAS_H
