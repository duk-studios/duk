/// 12/09/2023
/// color_palette.cpp

#include <duk_renderer/painters/color/color_palette.h>
#include <duk_renderer/components/transform.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace duk::renderer {

ColorPalette::ColorPalette(const ColorPaletteCreateInfo& colorPaletteCreateInfo) {

    auto rhi = colorPaletteCreateInfo.rhi;
    auto commandQueue = colorPaletteCreateInfo.commandQueue;
    auto painter = colorPaletteCreateInfo.painter;

    {
        StorageBufferCreateInfo transformUBOCreateInfo = {};
        transformUBOCreateInfo.rhi = rhi;
        transformUBOCreateInfo.commandQueue = commandQueue;
        m_transformSBO = std::make_unique<StorageBuffer<Transform, scene::MAX_OBJECTS>>(transformUBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData = Material{glm::vec4(1)};
        m_materialUBO = std::make_unique<UniformBuffer<Material>>(materialUBOCreateInfo);
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
        duk::rhi::RHI::DescriptorSetCreateInfo instanceDescriptorSet = {};
        instanceDescriptorSet.description = painter->instance_descriptor_set_description();

        auto expectedInstanceDescriptorSet = rhi->create_descriptor_set(instanceDescriptorSet);

        if (!expectedInstanceDescriptorSet) {
            throw std::runtime_error("failed to create ColorPalette instance descriptor set: " + expectedInstanceDescriptorSet.error().description());
        }

        m_instanceDescriptorSet = std::move(expectedInstanceDescriptorSet.value());

        m_instanceDescriptorSet->set(0, *m_transformSBO);
        m_instanceDescriptorSet->flush();
    }

    {
        duk::rhi::RHI::DescriptorSetCreateInfo globalDescriptorSet = {};
        globalDescriptorSet.description = painter->global_descriptor_set_description();

        auto expectedGlobalDescriptorSet = rhi->create_descriptor_set(globalDescriptorSet);

        if (!expectedGlobalDescriptorSet) {
            throw std::runtime_error("failed to create ColorPalette global descriptor set: " + expectedGlobalDescriptorSet.error().description());
        }

        m_globalDescriptorSet = std::move(expectedGlobalDescriptorSet.value());

        m_globalDescriptorSet->set(0, *m_cameraUBO);
        m_globalDescriptorSet->set(1, *m_materialUBO);
        m_globalDescriptorSet->flush();
    }
}

void ColorPalette::set_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
}

void ColorPalette::insert_instance(const Palette::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
}

void ColorPalette::apply(duk::rhi::CommandBuffer* commandBuffer) {

    m_transformSBO->flush();
    m_cameraUBO->flush();
    m_materialUBO->flush();

    commandBuffer->bind_descriptor_set(m_globalDescriptorSet.get(), 0);
    commandBuffer->bind_descriptor_set(m_instanceDescriptorSet.get(), 1);
}

void ColorPalette::clear() {
    m_transformSBO->clear();
}

}