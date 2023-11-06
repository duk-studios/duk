/// 06/11/2023
/// lighting.h

#ifndef DUK_RENDERER_LIGHTING_H
#define DUK_RENDERER_LIGHTING_H

#include <duk_scene/scene.h>
#include <glm/glm.hpp>

namespace duk::renderer {

struct LightValue {
    glm::vec3 color;
    float intensity;
};

struct DirectionalLight {
    LightValue value;
};

struct PointLight {
    LightValue value;
    float radius;
};

}

#endif // DUK_RENDERER_LIGHTING_H

