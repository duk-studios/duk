//
// Created by Ricardo on 07/04/2024.
//

#include <duk_renderer/components/canvas.h>
#include <duk_renderer/material/material.h>

namespace duk::renderer {

void update_canvas(const objects::Component<Canvas>& canvas, uint32_t width, uint32_t height) {
    canvas->size = glm::ivec2(width, height);
    canvas->proj = glm::ortho(0.f, (float)width, 0.f, (float)height);
}

void update_canvas_transform(const duk::objects::Component<Canvas>& canvas, duk::objects::Component<CanvasTransform>& canvasTransform) {
    const auto anchorPosition = canvasTransform->anchor * glm::vec2(canvas->size);
    const auto position = canvasTransform->position + anchorPosition;
    const auto halfSize = canvasTransform->size / 2.0f;
    const auto pivotOffset = halfSize * ((canvasTransform->pivot * 2.0f) - 1.0f);
    canvasTransform->model = glm::translate(glm::mat4(1), glm::vec3(position - pivotOffset, 0.0));
    canvasTransform->model = glm::scale(canvasTransform->model, glm::vec3(canvasTransform->scale, 1));

    auto object = canvasTransform.object();
    if (auto material = find_material(object)) {
        material->set(object.id(), "uTransform", "model", canvasTransform->model);
    }
}

}// namespace duk::renderer
