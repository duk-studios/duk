/// 13/10/2023
/// phong_palette.cpp

#include <duk_renderer/painters/phong/phong_palette.h>
#include <duk_renderer/painters/phong/phong_painter.h>
#include <duk_renderer/painters/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

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
        m_transformSBO = std::make_unique<phong::TransformSBO>(transformUBOCreateInfo);
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
    }
}

void PhongPalette::clear_instances() {
    m_transformSBO->clear();
}

void PhongPalette::insert_instance(const Palette::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
}

void PhongPalette::flush_instances() {
    m_transformSBO->flush();
}

void PhongPalette::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {

    // updates current camera UBO
    m_descriptorSet->set(0, *params.globalDescriptors->camera_ubo());
    m_descriptorSet->set(2, *params.globalDescriptors->lights_ubo());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

}