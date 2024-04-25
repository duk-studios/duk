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
        uInstances = 1,
        uBaseColor = 2,
        uMaterial = 3
    };

    static constexpr uint32_t kDescriptorCount = 4;

    struct Instance {
        glm::mat4x4 model;
    };

    struct Material {
        glm::vec4 color;
    };
};

}// namespace duk::renderer

#endif// DUK_RENDERER_SPRITE_COLOR_DESCRIPTORS_H
