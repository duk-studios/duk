/// 12/09/2023
/// color_palette.cpp

#include <duk_renderer/painters/color/color_palette.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

ColorPalette::ColorPalette(const ColorPaletteCreateInfo& colorPaletteCreateInfo) {

    auto rhi = colorPaletteCreateInfo.rhi;
    auto commandQueue = colorPaletteCreateInfo.commandQueue;
    auto painter = colorPaletteCreateInfo.painter;

    {
        duk::rhi::RHI::BufferCreateInfo transformUBOCreateInfo = {};
        transformUBOCreateInfo.type = duk::rhi::Buffer::Type::UNIFORM;
        transformUBOCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
        transformUBOCreateInfo.elementCount = 1;
        transformUBOCreateInfo.elementSize = sizeof(ColorPainter::Transform);
        transformUBOCreateInfo.commandQueue = commandQueue;

        auto expectedTransformUBO = rhi->create_buffer(transformUBOCreateInfo);

        if (!expectedTransformUBO) {
            throw std::runtime_error("failed to create ColorPalette::Transform UBO: " + expectedTransformUBO.error().description());
        }

        m_transformUBO = std::move(expectedTransformUBO.value());
        m_transformData.model = glm::mat4(1);
    }

    {
        duk::rhi::RHI::BufferCreateInfo materialUboCreateInfo = {};
        materialUboCreateInfo.type = duk::rhi::Buffer::Type::UNIFORM;
        materialUboCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
        materialUboCreateInfo.elementCount = 1;
        materialUboCreateInfo.elementSize = sizeof(ColorPainter::Material);
        materialUboCreateInfo.commandQueue = commandQueue;

        auto expectedMaterialUBO = rhi->create_buffer(materialUboCreateInfo);

        if (!expectedMaterialUBO) {
            throw std::runtime_error("failed to create ColorPalette::Material UBO: " + expectedMaterialUBO.error().description());
        }

        m_materialUBO = std::move(expectedMaterialUBO.value());
        m_materialData.color = glm::vec4(1);
    }

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = painter->instance_descriptor_set_description();

        auto expectedInstanceDescriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);

        if (!expectedInstanceDescriptorSet) {
            throw std::runtime_error("failed to create ColorPalette instance descriptor set: " + expectedInstanceDescriptorSet.error().description());
        }

        m_instanceDescriptorSet = std::move(expectedInstanceDescriptorSet.value());

        m_instanceDescriptorSet->set(0, duk::rhi::Descriptor::uniform_buffer(m_transformUBO.get()));
        m_instanceDescriptorSet->set(1, duk::rhi::Descriptor::uniform_buffer(m_materialUBO.get()));

        m_instanceDescriptorSet->flush();
    }
}

void ColorPalette::set_color(const glm::vec4& color) {
    m_materialData.color = color;
}

void ColorPalette::update(const Palette::UpdateParams& params) {
    m_transformData.model = duk::renderer::model_matrix_3d(params.object);
}

void ColorPalette::apply(duk::rhi::CommandBuffer* commandBuffer) {

    m_transformUBO->write(m_transformData);
    m_transformUBO->flush();

    m_materialUBO->write(m_materialData);
    m_materialUBO->flush();

    commandBuffer->bind_descriptor_set(m_instanceDescriptorSet.get(), 0);
}

}