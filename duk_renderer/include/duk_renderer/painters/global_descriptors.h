/// 13/10/2023
/// global_buffers.h

#ifndef DUK_RENDERER_GLOBAL_BUFFERS_H
#define DUK_RENDERER_GLOBAL_BUFFERS_H

#include <duk_scene/scene.h>

#include <duk_renderer/painters/camera.h>
#include <duk_renderer/painters/lights.h>

namespace duk::renderer {

struct GlobalDescriptorsCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class GlobalDescriptors {
public:

    explicit GlobalDescriptors(const GlobalDescriptorsCreateInfo& globalDescriptorsCreateInfo);

    void update_camera(const duk::scene::Object& cameraObject);

    CameraUBO* camera_ubo();

    LightsUBO* lights_ubo();

private:
    std::unique_ptr<CameraUBO> m_cameraUBO;
    std::unique_ptr<LightsUBO> m_lightsUBO;
};

}

#endif // DUK_RENDERER_GLOBAL_BUFFERS_H

