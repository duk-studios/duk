/// 13/10/2023
/// global_buffers.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>

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
        lightUboCreateInfo.initialData = {};
        m_lightsUBO = std::make_unique<globals::LightsUBO>(lightUboCreateInfo);
    }
}

void GlobalDescriptors::update_cameras(duk::scene::Objects& objects) {
    uint32_t cameraCount = 0;
    for (auto object: objects.all_with<Camera>()) {
        cameraCount++;
        if (cameraCount > 1) {
            duk::log::warn("multiple cameras detected, unsupported at the moment");
            break;
        }
        auto camera = object.component<Camera>();
        auto& uboData = m_cameraUBO->data();
        uboData.proj = camera->proj;
        uboData.view = camera->view;
        uboData.invView = camera->invView;
        uboData.vp = camera->vp;
        m_cameraUBO->flush();
    }
    if (cameraCount == 0) {
        duk::log::warn("no camera found in objects, nothing will be rendered");
    }
}

void GlobalDescriptors::update_lights(duk::scene::Objects& objects) {
    auto& lights = m_lightsUBO->data();
    lights.directionalLightCount = 0;
    for (auto object: objects.all_with<DirectionalLight>()) {
        auto directionalLightComponent = object.component<DirectionalLight>();
        auto& directionalLight = lights.directionalLights[lights.directionalLightCount++];
        directionalLight.direction = forward(*object.component<Transform>());
        directionalLight.value.color = directionalLightComponent->value.color;
        directionalLight.value.intensity = directionalLightComponent->value.intensity;

        if (lights.directionalLightCount >= sizeof globals::Lights::directionalLights) {
            break;
        }
    }

    lights.pointLightCount = 0;
    for (auto object: objects.all_with<PointLight>()) {
        auto pointLightComponent = object.component<PointLight>();
        auto positionComponent = object.component<Position3D>();
        auto& pointLight = lights.pointLights[lights.pointLightCount++];
        pointLight.position = positionComponent ? positionComponent->value : glm::vec3(0);
        pointLight.linear = 0.14f;
        pointLight.quadratic = 0.07f;
        pointLight.value.color = pointLightComponent->value.color;
        pointLight.value.intensity = pointLightComponent->value.intensity;

        if (lights.pointLightCount >= sizeof globals::Lights::pointLights) {
            break;
        }
    }

    m_lightsUBO->flush();
}

globals::CameraUBO* GlobalDescriptors::camera_ubo() {
    return m_cameraUBO.get();
}

globals::LightsUBO* GlobalDescriptors::lights_ubo() {
    return m_lightsUBO.get();
}

}// namespace duk::renderer