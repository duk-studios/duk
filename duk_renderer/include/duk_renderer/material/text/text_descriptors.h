#ifndef DUK_RENDERER_TEXT_DESCRIPTORS_H
#define DUK_RENDERER_TEXT_DESCRIPTORS_H

#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_renderer/material/text/text_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class TextDescriptors {
public:
    enum Bindings : uint32_t {
        uCamera = 0,
        uInstance = 1,
        uMaterial = 2,
        uBaseColor = 3
    };

    static constexpr uint32_t kDescriptorCount = 4;

    struct Material {
        glm::vec4 color;
    };

    struct Transform {
        glm::mat4x4 model;
    };
};

}// namespace duk::renderer

#endif// DUK_RENDERER_TEXT_DESCRIPTORS_H
