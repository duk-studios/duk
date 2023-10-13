/// 13/10/2023
/// phong_palette.cpp

#include <duk_renderer/painters/phong/phong_palette.h>
#include <duk_renderer/painters/phong/phong_painter.h>
#include <duk_renderer/components/transform.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace duk::renderer {

PhongPalette::PhongPalette(const PhongPaletteCreateInfo& phongPaletteCreateInfo) {
    auto rhi = phongPaletteCreateInfo.rhi;
    auto commandQueue = phongPaletteCreateInfo.commandQueue;
    auto painter = phongPaletteCreateInfo.painter;

    {
        StorageBufferCreateInfo transformUBOCreateInfo = {};
        transformUBOCreateInfo.rhi = rhi;
        transformUBOCreateInfo.commandQueue = commandQueue;
        transformUBOCreateInfo.initialSize = 1;
        m_transformSBO = std::make_unique<StorageBuffer<Transform>>(transformUBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<DirectionalLight> lightUboCreateInfo = {};
        lightUboCreateInfo.rhi = rhi;
        lightUboCreateInfo.commandQueue = commandQueue;
        lightUboCreateInfo.initialData.value.color = glm::vec3(1);
        lightUboCreateInfo.initialData.value.intensity = 1.0f;
        lightUboCreateInfo.initialData.direction = glm::vec3(1);
        m_lightUBO = std::make_unique<UniformBuffer<DirectionalLight>>(lightUboCreateInfo);
    }

    {
        UniformBufferCreateInfo<CameraMatrices> cameraUBOCreateInfo = {};
        cameraUBOCreateInfo.rhi = rhi;
        cameraUBOCreateInfo.commandQueue = commandQueue;
        m_cameraUBO = std::make_unique<CameraUBO>(cameraUBOCreateInfo);

        auto& cameraMatrices = m_cameraUBO->data();
        cameraMatrices.proj = glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 1000.f);
        cameraMatrices.view = glm::lookAt(glm::vec3(0, 0, 40), glm::vec3(0), glm::vec3(0, 1, 0));
        cameraMatrices.vp = cameraMatrices.proj * cameraMatrices.view;

        m_cameraUBO->flush();
    }

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = painter->descriptor_set_description();

        auto expectedDescriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);

        if (!expectedDescriptorSet) {
            throw std::runtime_error("failed to create ColorPalette descriptor set: " + expectedDescriptorSet.error().description());
        }

        m_descriptorSet = std::move(expectedDescriptorSet.value());

        m_descriptorSet->set(0, *m_cameraUBO);
        m_descriptorSet->set(1, *m_transformSBO);
        m_descriptorSet->set(2, *m_lightUBO);
        m_descriptorSet->flush();
    }
}

void PhongPalette::insert_instance(const Palette::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
}

void PhongPalette::apply(duk::rhi::CommandBuffer* commandBuffer) {
    m_transformSBO->flush();
    m_cameraUBO->flush();
    m_lightUBO->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

void PhongPalette::clear() {
    m_transformSBO->clear();
}

}