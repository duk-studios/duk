/// 13/10/2023
/// global_buffers.cpp

#include <duk_renderer/painters/globals/global_descriptors.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

GlobalDescriptors::GlobalDescriptors(const GlobalDescriptorsCreateInfo& globalDescriptorsCreateInfo) {
    auto rhi = globalDescriptorsCreateInfo.rhi;
    auto commandQueue = globalDescriptorsCreateInfo.commandQueue;
    {
        UniformBufferCreateInfo<globals::CameraMatrices> cameraUBOCreateInfo = {};
        cameraUBOCreateInfo.rhi = rhi;
        cameraUBOCreateInfo.commandQueue = commandQueue;
        m_cameraUBO = std::make_unique<globals::CameraUBO>(cameraUBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<globals::Lights> lightUboCreateInfo = {};
        lightUboCreateInfo.rhi = rhi;
        lightUboCreateInfo.commandQueue = commandQueue;
        lightUboCreateInfo.initialData.directionalLights[0].value.color = glm::vec3(1);
        lightUboCreateInfo.initialData.directionalLights[0].value.intensity = 1.0f;
        lightUboCreateInfo.initialData.directionalLights[0].direction = glm::vec3(0, 0, -1);
        m_lightsUBO = std::make_unique<globals::LightsUBO>(lightUboCreateInfo);
    }
}

void GlobalDescriptors::update_camera(const scene::Object& cameraObject) {
    auto& camera = m_cameraUBO->data();
    camera.proj = calculate_projection(cameraObject);

    // an inverse view is simply the normal model matrix
    camera.invView = model_matrix_3d(cameraObject);

    // inverse it to have the proper view matrix
    camera.view = glm::inverse(camera.invView);

    camera.vp = camera.proj * camera.view;

    m_cameraUBO->flush();
}

globals::CameraUBO* GlobalDescriptors::camera_ubo() {
    return m_cameraUBO.get();
}

globals::LightsUBO* GlobalDescriptors::lights_ubo() {
    return m_lightsUBO.get();
}

}