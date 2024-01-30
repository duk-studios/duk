#ifndef DUK_RENDERER_SPRITE_COLOR_TYPES_H
#define DUK_RENDERER_SPRITE_COLOR_TYPES_H

#include <duk_renderer/resources/materials/uniform_buffer.h>
#include <duk_renderer/resources/materials/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::sprite_color {

struct Material {
    glm::vec4 color;
};

using MaterialUBO = UniformBuffer<Material>;

} // namespace duk::renderer::sprite_color

#endif // DUK_RENDERER_SPRITE_COLOR_TYPES_H
