/// 20/08/2023
/// transform.cpp

#include <duk_renderer/components/transform.h>

namespace duk::renderer {

glm::vec3 forward(const duk::objects::Component<Transform>& transform) {
    auto matrices = transform.component<Matrices>();
    return glm::vec3(matrices->model * glm::vec4(0, 0, -1, 1));
}

glm::vec3 world_position(const duk::objects::Component<Transform>& transform) {
    auto matrices = transform.component<Matrices>();
    return glm::vec3(matrices->model[3]);
}

}// namespace duk::renderer