/// 13/10/2023
/// phong_material.cpp

#include <duk_renderer/resources/materials/phong/phong_material.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

static const PhongShaderDataSource kPhongShaderDataSource;

duk::hash::Hash PhongMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, baseColor);
    duk::hash::hash_combine(hash, baseColorImage.id().value());
    duk::hash::hash_combine(hash, baseColorSampler);
    duk::hash::hash_combine(hash, shininess);
    duk::hash::hash_combine(hash, shininessImage.id().value());
    duk::hash::hash_combine(hash, shininessSampler);
    return hash;
}

PhongMaterialDataSource::PhongMaterialDataSource() :
    MaterialDataSource(MaterialType::PHONG),
    baseColor(),
    baseColorSampler(),
    shininess(1),
    shininessSampler() {

}

PhongMaterial::PhongMaterial(const PhongMaterialCreateInfo& phongMaterialCreateInfo) :
    MeshMaterial(phongMaterialCreateInfo.renderer, &kPhongShaderDataSource),
    m_descriptorSet({phongMaterialCreateInfo.renderer->rhi(), &kPhongShaderDataSource}),
    m_baseColorSampler(),
    m_shininessSampler() {
    auto rhi = phongMaterialCreateInfo.renderer->rhi();
    auto commandQueue = phongMaterialCreateInfo.renderer->main_command_queue();
    auto materialDataSource = phongMaterialCreateInfo.phongMaterialDataSource;

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
        materialUBOCreateInfo.initialData.color = materialDataSource->baseColor;
        materialUBOCreateInfo.initialData.shininess = materialDataSource->shininess;
        m_materialUBO = std::make_unique<phong::MaterialUBO>(materialUBOCreateInfo);

        update_base_color_image(materialDataSource->baseColorImage, materialDataSource->baseColorSampler);
        update_shininess_image(materialDataSource->shininessImage, materialDataSource->shininessSampler);
    }

    m_descriptorSet.set(PhongDescriptorSet::Bindings::uTransform, *m_transformSBO);
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uMaterial, *m_materialUBO);

}

void PhongMaterial::clear_instances() {
    m_transformSBO->clear();
}

void PhongMaterial::insert_instance(const InsertInstanceParams& params) {
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

void PhongMaterial::update_base_color_image(const ImageResource& baseColorImage, const duk::rhi::Sampler& sampler) {
    m_baseColorImage = baseColorImage;
    m_baseColorSampler = sampler;
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uBaseColor, duk::rhi::Descriptor::image_sampler(m_baseColorImage.get(), duk::rhi::Image::Layout::SHADER_READ_ONLY, m_baseColorSampler));
}

void PhongMaterial::update_base_color(const glm::vec3& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void PhongMaterial::update_shininess_image(const ImageResource& shininessImage, const rhi::Sampler& sampler) {
    m_shininessImage = shininessImage;
    m_shininessSampler = sampler;
    m_descriptorSet.set(PhongDescriptorSet::Bindings::uSpecular, duk::rhi::Descriptor::image_sampler(m_shininessImage.get(), duk::rhi::Image::Layout::SHADER_READ_ONLY, m_shininessSampler));
}

void PhongMaterial::update_shininess(float shininess) {
    m_materialUBO->data().shininess = shininess;
    m_materialUBO->flush();
}

}