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

}// namespace duk::ui

namespace duk::type {
// clang-format off
template<>
struct Type<duk::ui::Image> : Class<duk::ui::Image,
    Member<"image", &duk::ui::Image::image>,
    Member<"sampler", &duk::ui::Image::sampler>,
    Member<"color", &duk::ui::Image::color>,
    Member<"keepAspectRatio", &duk::ui::Image::keepAspectRatio>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_UI_IMAGE_H
