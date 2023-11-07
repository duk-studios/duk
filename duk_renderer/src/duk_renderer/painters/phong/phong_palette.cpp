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
        StorageBufferCreateInfo transformSBOCreateInfo = {};
        transformSBOCreateInfo.rhi = rhi;
        transformSBOCreateInfo.commandQueue = commandQueue;
        transformSBOCreateInfo.initialSize = 1;
        m_transformSBO = std::make_unique<phong::TransformSBO>(transformSBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<phong::Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData = {};
        m_materialUBO = std::make_unique<phong::MaterialUBO>(materialUBOCreateInfo);
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
        m_descriptorSet->set(3, *m_materialUBO);
    }
}

void PhongPalette::clear_instances() {
    m_transformSBO->clear();
}

void PhongPalette::insert_instance(const Palette::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
    transform.invModel = glm::inverse(transform.model);
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

void PhongPalette::update_diffuse(const glm::vec3& diffuse) {
    m_materialUBO->data().diffuse = diffuse;
    m_materialUBO->flush();
}

void PhongPalette::update_specular(const glm::vec3& specular) {
    m_materialUBO->data().specular = specular;
    m_materialUBO->flush();
}

void PhongPalette::update_ambient(const glm::vec3& ambient) {
    m_materialUBO->data().ambient = ambient;
    m_materialUBO->flush();
}

void PhongPalette::update_shininess(float shininess) {
    m_materialUBO->data().shininess = shininess;
    m_materialUBO->flush();
}

void PhongPalette::update_material(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient, float shininess) {
    m_materialUBO->data().diffuse = diffuse;
    m_materialUBO->data().specular = specular;
    m_materialUBO->data().ambient = ambient;
    m_materialUBO->data().shininess = shininess;
    m_materialUBO->flush();
}

}