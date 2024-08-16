/// 13/10/2023
/// global_buffers.h

#ifndef DUK_RENDERER_GLOBAL_DESCRIPTORS_H
#define DUK_RENDERER_GLOBAL_DESCRIPTORS_H

#include <duk_objects/objects.h>

#include <duk_renderer/material/globals/camera_types.h>
#include <duk_renderer/material/globals/lights_types.h>

namespace duk::renderer {

struct GlobalDescriptorsCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class GlobalDescriptors {
public:
    explicit GlobalDescriptors(const GlobalDescriptorsCreateInfo& globalDescriptorsCreateInfo);

    void update_cameras(duk::objects::Objects& objects);

    void update_lights(duk::objects::Objects& objects);

    globals::ViewProjUBO* camera_ubo();

    globals::LightsUBO* lights_ubo();

private:
    std::unique_ptr<globals::ViewProjUBO> m_cameraUBO;
    std::unique_ptr<globals::LightsUBO> m_lightsUBO;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_GLOBAL_DESCRIPTORS_H
