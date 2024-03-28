#ifndef DUK_RENDERER_SPRITE_COLOR_DESCRIPTORS_H
#define DUK_RENDERER_SPRITE_COLOR_DESCRIPTORS_H

#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_renderer/material/sprites/sprite_color/sprite_color_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class SpriteColorDescriptors {
public:
    enum Bindings : uint32_t {
        uCamera = 0,
        uBaseColor = 1,
        uMaterial = 2
    };

    static constexpr uint32_t kDescriptorCount = 3;

    struct Material {
        glm::vec4 color;
    };
};

}// namespace duk::renderer

#endif// DUK_RENDERER_SPRITE_COLOR_DESCRIPTORS_H
