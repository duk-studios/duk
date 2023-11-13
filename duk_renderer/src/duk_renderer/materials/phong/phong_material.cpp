/// 13/10/2023
/// phong_material.cpp

#include <duk_renderer/materials/phong/phong_material.h>
#include <duk_renderer/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

PhongMaterial::PhongMaterial(const PhongMaterialCreateInfo& phongMaterialCreateInfo) :
    Material(phongMaterialCreateInfo.renderer, phongMaterialCreateInfo.shaderDataSource),
    m_descriptorSet({phongMaterialCreateInfo.renderer->rhi(), phongMaterialCreateInfo.shaderDataSource}) {
    auto rhi = phongMaterialCreateInfo.renderer->rhi();
    auto commandQueue = phongMaterialCreateInfo.renderer->main_command_queue();

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

void PhongMaterial::clear_instances() {
    m_transformSBO->clear();
}

void PhongMaterial::insert_instance(const Material::InsertInstanceParams& params) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
    transform.invModel = glm::inverse(transform.model);
}

void PhongMaterial::flush_instances() {
    m_transformSBO->flush();
}

void PhongMaterial::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {

    // updates current camera UBO
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uCamera, *params.globalDescriptors->camera_ubo());
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uLights, *params.globalDescriptors->lights_ubo());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet.flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.handle(), 0);
}

void PhongMaterial::update_base_color_image(duk::rhi::Image* image, const duk::rhi::Sampler& sampler) {
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uBaseColor, duk::rhi::Descriptor::image_sampler(image, duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler));
}

void PhongMaterial::update_base_color(const glm::vec3& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void PhongMaterial::update_shininess(float shininess) {
    m_materialUBO->data().shininess = shininess;
    m_materialUBO->flush();
}



}