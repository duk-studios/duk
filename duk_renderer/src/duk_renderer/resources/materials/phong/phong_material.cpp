/// 13/10/2023
/// phong_material.cpp

#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/phong/phong_material.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/pools/image_pool.h>

namespace duk::renderer {

namespace detail {

static const PhongShaderDataSource kPhongShaderDataSource;

}

duk::hash::Hash PhongMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, albedo);
    duk::hash::hash_combine(hash, albedoTexture);
    duk::hash::hash_combine(hash, specular);
    duk::hash::hash_combine(hash, specularTexture);
    return hash;
}

PhongMaterialDataSource::PhongMaterialDataSource() :
    albedo(1),
    albedoTexture(),
    specular(1),
    specularTexture() {

}

const rhi::ShaderDataSource* PhongMaterialDataSource::shader_data_source() const {
    return &detail::kPhongShaderDataSource;
}

std::unique_ptr<MaterialDescriptorSet> PhongMaterialDataSource::create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const {
    PhongMaterialDescriptorSetCreateInfo phongMaterialDescriptorSetCreateInfo = {};
    phongMaterialDescriptorSetCreateInfo.renderer = materialDescriptorSetCreateInfo.renderer;
    phongMaterialDescriptorSetCreateInfo.phongMaterialDataSource = this;
    return std::make_unique<PhongMaterialDescriptorSet>(phongMaterialDescriptorSetCreateInfo);
}

PhongInstanceBuffer::PhongInstanceBuffer(const PhongInstanceBufferCreateInfo& phongInstanceBufferCreateInfo) {

    auto rhi = phongInstanceBufferCreateInfo.renderer->rhi();
    auto commandQueue = phongInstanceBufferCreateInfo.renderer->main_command_queue();

    StorageBufferCreateInfo transformSBOCreateInfo = {};
    transformSBOCreateInfo.rhi = rhi;
    transformSBOCreateInfo.commandQueue = commandQueue;
    transformSBOCreateInfo.initialSize = 1;
    m_transformSBO = std::make_unique<StorageBuffer<PhongDescriptors::Transform>>(transformSBOCreateInfo);
}

void PhongInstanceBuffer::insert(const scene::Object& object) {
    auto& transform = m_transformSBO->next();
    transform.model = duk::renderer::model_matrix_3d(object);
    transform.invModel = glm::inverse(transform.model);
}

void PhongInstanceBuffer::clear() {
    m_transformSBO->clear();
}

void PhongInstanceBuffer::flush() {
    m_transformSBO->flush();
}

StorageBuffer<PhongDescriptors::Transform>* PhongInstanceBuffer::transform_buffer() {
    return m_transformSBO.get();
}

PhongMaterialDescriptorSet::PhongMaterialDescriptorSet(const PhongMaterialDescriptorSetCreateInfo& phongMaterialDescriptorSetCreateInfo) {
    auto renderer = phongMaterialDescriptorSetCreateInfo.renderer;
    auto rhi = renderer->rhi();
    auto commandQueue = renderer->main_command_queue();
    auto materialDataSource = phongMaterialDescriptorSetCreateInfo.phongMaterialDataSource;
    auto shaderDataSource = materialDataSource->shader_data_source();

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = shaderDataSource->descriptor_set_descriptions().at(0);
        m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);
    }

    {
        PhongInstanceBufferCreateInfo instanceBufferCreateInfo = {};
        instanceBufferCreateInfo.renderer = renderer;
        m_instanceBuffer = std::make_unique<PhongInstanceBuffer>(instanceBufferCreateInfo);
        m_descriptorSet->set(PhongDescriptors::uTransform, m_instanceBuffer->transform_buffer()->descriptor());
    }

    {
        UniformBufferCreateInfo<PhongDescriptors::Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        m_materialUBO = std::make_unique<UniformBuffer<PhongDescriptors::Material>>(materialUBOCreateInfo);
        m_descriptorSet->set(PhongDescriptors::uMaterial, m_materialUBO->descriptor());
    }

    set_albedo_texture(materialDataSource->albedoTexture);
    set_albedo_color(materialDataSource->albedo);
    set_specular_texture(materialDataSource->specularTexture);
    set_specular_value(materialDataSource->specular);
}

void PhongMaterialDescriptorSet::set_albedo_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void PhongMaterialDescriptorSet::set_albedo_texture(const Texture& albedo) {
    m_albedo = albedo;
}

void PhongMaterialDescriptorSet::set_specular_value(float value) {
    m_materialUBO->data().shininess = value;
    m_materialUBO->flush();
}

void PhongMaterialDescriptorSet::set_specular_texture(const Texture& specular) {
    m_specular = specular;
}

uint32_t PhongMaterialDescriptorSet::size() const {
    return PhongDescriptors::kDescriptorCount;
}

ImageResource& PhongMaterialDescriptorSet::image_at(uint32_t index) {
    switch (index) {
        case PhongDescriptors::uBaseColor:
            return m_albedo.image();
        case PhongDescriptors::uSpecular:
            return m_specular.image();
        default:
            throw std::invalid_argument(fmt::format("Descriptor at index {} is not an image", index));
    }
}

bool PhongMaterialDescriptorSet::is_image(uint32_t index) {
    return index == PhongDescriptors::uBaseColor || index == PhongDescriptors::uSpecular;
}

void PhongMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& drawParams) {
    // update global UBOs
    m_descriptorSet->set(PhongDescriptors::uCamera, drawParams.globalDescriptors->camera_ubo()->descriptor());
    m_descriptorSet->set(PhongDescriptors::uLights, drawParams.globalDescriptors->lights_ubo()->descriptor());

    // These might have changed
    m_descriptorSet->set(PhongDescriptors::uBaseColor, m_albedo.descriptor());
    m_descriptorSet->set(PhongDescriptors::uSpecular, m_specular.descriptor());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

InstanceBuffer* PhongMaterialDescriptorSet::instance_buffer() {
    return m_instanceBuffer.get();
}

}
