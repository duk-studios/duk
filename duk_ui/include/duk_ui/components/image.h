//
// Created by Ricardo on 26/08/2024.
//

#ifndef DUK_UI_IMAGE_H
#define DUK_UI_IMAGE_H

#include <duk_renderer/image/image.h>
#include <duk_resource/handle.h>

namespace duk::ui {

struct Image {
    duk::resource::Handle<duk::renderer::Image> image;
    duk::rhi::Sampler sampler;
    glm::vec4 color;
    bool keepAspectRatio;
    uint64_t hash = 0;
};

}

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::ui::Image& image) {
    from_json_member(json, "image", image.image);
    from_json_member(json, "sampler", image.sampler);
    from_json_member(json, "color", image.color);
    from_json_member(json, "keepAspectRatio", image.keepAspectRatio);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::ui::Image& image) {
    to_json_member(document, json, "image", image.image);
    to_json_member(document, json, "sampler", image.sampler);
    to_json_member(document, json, "color", image.color);
    to_json_member(document, json, "keepAspectRatio", image.keepAspectRatio);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::ui::Image& image) {
    solver->solve(image.image);
    solver->solve(image.color);
}

}

#endif //DUK_UI_IMAGE_H
