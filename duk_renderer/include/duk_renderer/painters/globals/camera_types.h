#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <glm/glm.hpp>

namespace duk::renderer::globals {

struct CameraMatrices {
    glm::mat4x4 view;
    glm::mat4x4 invView;
    glm::mat4x4 proj;
    glm::mat4x4 vp;
};

using CameraUBO = UniformBuffer<CameraMatrices>;

} // namespace duk::renderer::globals
