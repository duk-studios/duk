#ifndef DUK_RENDERER_PHONG_DESCRIPTORS_H
#define DUK_RENDERER_PHONG_DESCRIPTORS_H

#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_renderer/material/phong/phong_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class PhongDescriptors {
public:
    enum Bindings : uint32_t {
        uCamera = 0,
        uTransform = 1,
        uLights = 2,
        uMaterial = 3,
        uBaseColor = 4,
        uSpecular = 5
    };

    static constexpr uint32_t kDescriptorCount = 6;

    struct Material {
        glm::vec3 color;
        float shininess;
    };

    struct Transform {
        glm::mat4x4 model;
        glm::mat4x4 invModel;
    };
};

}// namespace duk::renderer

#endif// DUK_RENDERER_PHONG_DESCRIPTORS_H
