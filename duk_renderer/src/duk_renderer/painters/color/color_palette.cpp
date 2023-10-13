/// 12/09/2023
/// color_palette.cpp

#include <duk_renderer/painters/color/color_palette.h>
#include <duk_renderer/painters/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

ColorPalette::ColorPalette(const ColorPaletteCreateInfo& colorPaletteCreateInfo) {

    auto rhi = colorPaletteCreateInfo.rhi;
    auto commandQueue = colorPaletteCreateInfo.commandQueue;
    auto painter = colorPaletteCreateInfo.painter;

    {
        StorageBufferCreateInfo transformUBOCreateInfo = {};
        transformUBOCreateInfo.rhi = rhi;
        transformUBOCreateInfo.commandQueue = commandQueue;
        transformUBOCreateInfo.initialSize = 1;
        m_transformSBO = std::make_unique<StorageBuffer<Transform>>(transformUBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData = Material{glm::vec4(1)};
        m_materialUBO = std::make_unique<UniformBuffer<Material>>(materialUBOCreateInfo);
    }

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = painter->descriptor_set_description();

        auto expectedDescriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);

        if (!expectedDescriptorSet) {
            throw std::runtime_error("failed to create ColorPalette descriptor set: " + expectedDescriptorSet.error().description());
        }

        m_descriptorSet = std::move(expectedDescriptorSet.value());
        m_descriptorSet->set(1, *m_transformSBO);
        m_descriptorSet->set(2, *m_materialUBO);
    }
}

void ColorPalette::set_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void ColorPalette::clear_instances() {
    m_transformSBO->clear();
}

void ColorPalette::insert_instance(const Palette::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
}

void ColorPalette::flush_instances() {
    m_transformSBO->flush();
}

void ColorPalette::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {

    // updates current camera UBO
    m_descriptorSet->set(0, *params.globalDescriptors->camera_ubo());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

}