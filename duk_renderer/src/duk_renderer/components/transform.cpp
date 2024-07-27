/// 20/08/2023
/// transform.cpp

#include <duk_renderer/components/transform.h>

namespace duk::renderer {

glm::vec3 forward(const Transform& transform) {
    return glm::vec3(transform.model * glm::vec4(0, 0, -1, 1));
}

glm::vec3 world_position(const Transform& transform) {
    return glm::vec3(transform.model[3]);
}

}// namespace duk::renderer