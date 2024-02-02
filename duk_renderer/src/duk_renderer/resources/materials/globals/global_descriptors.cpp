/// 13/10/2023
/// global_buffers.cpp

#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/lighting.h>

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

void GlobalDescriptors::update_lights(duk::scene::Scene* scene) {
    auto& lights = m_lightsUBO->data();
    lights.directionalLightCount = 0;
    for (auto object : scene->objects_with_components<DirectionalLight>()) {
        auto directionalLightComponent = object.component<DirectionalLight>();
        auto& directionalLight = lights.directionalLights[lights.directionalLightCount++];
        directionalLight.direction = forward_direction_3d(object);
        directionalLight.value.color = directionalLightComponent->value.color;
        directionalLight.value.intensity = directionalLightComponent->value.intensity;

        if (lights.directionalLightCount >= sizeof globals::Lights::directionalLights) {
            break;
        }
    }

    lights.pointLightCount = 0;
    for (auto object : scene->objects_with_components<PointLight, Position3D>()) {
        auto pointLightComponent = object.component<PointLight>();
        auto positionComponent = object.component<Position3D>();
        auto& pointLight = lights.pointLights[lights.pointLightCount++];
        pointLight.position = positionComponent->value;
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

}