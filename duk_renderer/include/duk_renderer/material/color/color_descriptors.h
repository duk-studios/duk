#ifndef DUK_RENDERER_COLOR_DESCRIPTORS_H
#define DUK_RENDERER_COLOR_DESCRIPTORS_H

#include <duk_renderer/material/color/color_shader_data_source.h>
#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class ColorDescriptors {
public:
    enum Bindings : uint32_t {
        uCamera = 0,
        uInstances = 1,
        uMaterial = 2,
        uBaseColor = 3
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

#endif// DUK_RENDERER_COLOR_DESCRIPTORS_H
