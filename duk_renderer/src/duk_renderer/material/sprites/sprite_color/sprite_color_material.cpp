/// 12/09/2023
/// color_material.cpp

#include <duk_renderer/image/image_pool.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/material/sprites/sprite_color/sprite_color_material.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

namespace detail {

static const SpriteColorShaderDataSource kSpriteColorShaderDataSource;

}

SpriteColorMaterialDataSource::SpriteColorMaterialDataSource()
    : color(1)
    , image(kWhiteImageId) {
}

const rhi::ShaderDataSource* SpriteColorMaterialDataSource::shader_data_source() const {
    return &detail::kSpriteColorShaderDataSource;
}

std::unique_ptr<MaterialDescriptorSet> SpriteColorMaterialDataSource::create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const {
    SpriteColorMaterialDescriptorSetCreateInfo spriteColorMaterialDescriptorSetCreateInfo = {};
    spriteColorMaterialDescriptorSetCreateInfo.renderer = materialDescriptorSetCreateInfo.renderer;
    spriteColorMaterialDescriptorSetCreateInfo.spriteColorMaterialDataSource = this;
    return std::make_unique<SpriteColorMaterialDescriptorSet>(spriteColorMaterialDescriptorSetCreateInfo);
}

hash::Hash SpriteColorMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, color);
    duk::hash::hash_combine(hash, image.id());
    return hash;
}

SpriteColorInstanceBuffer::SpriteColorInstanceBuffer(const SpriteColorInstanceBufferCreateInfo& spriteColorInstanceBufferCreateInfo) {
    auto rhi = spriteColorInstanceBufferCreateInfo.renderer->rhi();
    auto commandQueue = spriteColorInstanceBufferCreateInfo.renderer->main_command_queue();

    StorageBufferCreateInfo transformSBOCreateInfo = {};
    transformSBOCreateInfo.rhi = rhi;
    transformSBOCreateInfo.commandQueue = commandQueue;
    transformSBOCreateInfo.initialSize = 1;
    m_instancesSBO = std::make_unique<StorageBuffer<SpriteColorDescriptors::Instance>>(transformSBOCreateInfo);
}

void SpriteColorInstanceBuffer::insert(const duk::objects::Object& object) {
    auto transform = object.component<Transform>();
    auto& instance = m_instancesSBO->next();
    instance.model = transform->model;
}

void SpriteColorInstanceBuffer::clear() {
    m_instancesSBO->clear();
}

void SpriteColorInstanceBuffer::flush() {
    m_instancesSBO->flush();
}

const StorageBuffer<SpriteColorDescriptors::Instance>* SpriteColorInstanceBuffer::instances() const {
    return m_instancesSBO.get();
}

SpriteColorMaterialDescriptorSet::SpriteColorMaterialDescriptorSet(const SpriteColorMaterialDescriptorSetCreateInfo& spriteColorMaterialCreateInfo) {
    auto renderer = spriteColorMaterialCreateInfo.renderer;
    auto rhi = renderer->rhi();
    auto commandQueue = renderer->main_command_queue();
    auto materialDataSource = spriteColorMaterialCreateInfo.spriteColorMaterialDataSource;
    auto shaderDataSource = materialDataSource->shader_data_source();
    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = shaderDataSource->descriptor_set_descriptions().at(0);
        m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);
    }
    {
        SpriteColorInstanceBufferCreateInfo spriteColorInstanceBufferCreateInfo = {};
        spriteColorInstanceBufferCreateInfo.renderer = renderer;
        m_instanceBuffer = std::make_unique<SpriteColorInstanceBuffer>(spriteColorInstanceBufferCreateInfo);
        m_descriptorSet->set(SpriteColorDescriptors::uInstances, m_instanceBuffer->instances()->descriptor());
    }
    {
        UniformBufferCreateInfo<SpriteColorDescriptors::Material> materialUboCreateInfo = {};
        materialUboCreateInfo.rhi = rhi;
        materialUboCreateInfo.commandQueue = commandQueue;
        materialUboCreateInfo.initialData = {spriteColorMaterialCreateInfo.spriteColorMaterialDataSource->color};

        m_materialUBO = std::make_unique<UniformBuffer<SpriteColorDescriptors::Material>>(materialUboCreateInfo);
        m_descriptorSet->set(SpriteColorDescriptors::uMaterial, m_materialUBO->descriptor());
    }

    Texture texture{materialDataSource->image, {duk::rhi::Sampler::Filter::NEAREST, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE}};
    m_descriptorSet->set(SpriteColorDescriptors::uBaseColor, texture.descriptor());
}

void SpriteColorMaterialDescriptorSet::set_color(const glm::vec4& color) {
    auto& material = m_materialUBO->data();
    material.color = color;
    m_materialUBO->flush();
}

uint32_t SpriteColorMaterialDescriptorSet::size() const {
    return SpriteColorDescriptors::kDescriptorCount;
}

ImageResource& SpriteColorMaterialDescriptorSet::image_at(uint32_t index) {
    if (!is_image(index)) {
        throw std::invalid_argument(fmt::format("Descriptor at index {} is not an image", index));
    }
    return m_image;
}

bool SpriteColorMaterialDescriptorSet::is_image(uint32_t index) {
    return index == SpriteColorDescriptors::uBaseColor;
}

InstanceBuffer* SpriteColorMaterialDescriptorSet::instance_buffer() {
    return m_instanceBuffer.get();
}

void SpriteColorMaterialDescriptorSet::update(const DrawParams& params) {
    m_descriptorSet->set(SpriteColorDescriptors::uCamera, params.globalDescriptors->camera_ubo()->descriptor());
    m_descriptorSet->set(SpriteColorDescriptors::uInstances, m_instanceBuffer->instances()->descriptor());
    m_descriptorSet->flush();
}

void SpriteColorMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

}// namespace duk::renderer
