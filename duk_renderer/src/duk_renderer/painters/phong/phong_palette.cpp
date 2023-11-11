/// 13/10/2023
/// phong_palette.cpp

#include <duk_renderer/painters/phong/phong_palette.h>
#include <duk_renderer/painters/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

PhongPalette::PhongPalette(const PhongPaletteCreateInfo& phongPaletteCreateInfo) :
    m_descriptorSet({phongPaletteCreateInfo.renderer->rhi(), phongPaletteCreateInfo.shaderDataSource}) {
    auto rhi = phongPaletteCreateInfo.renderer->rhi();
    auto commandQueue = phongPaletteCreateInfo.renderer->main_command_queue();

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

    m_descriptorSet.set(PhongDescriptorSet::Bindings::uTransform, *m_transformSBO);
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uMaterial, *m_materialUBO);
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
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uCamera, *params.globalDescriptors->camera_ubo());
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uLights, *params.globalDescriptors->lights_ubo());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet.flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.handle(), 0);
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