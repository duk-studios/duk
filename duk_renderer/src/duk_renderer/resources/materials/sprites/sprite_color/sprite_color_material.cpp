/// 12/09/2023
/// color_material.cpp

#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_material.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/resources/builtin_resource_ids.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

namespace detail {

static const SpriteColorShaderDataSource kSpriteColorShaderDataSource;

}

SpriteColorMaterialDataSource::SpriteColorMaterialDataSource() :
    color(1),
    image(kWhiteImageId) {

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
        UniformBufferCreateInfo<SpriteColorDescriptors::Material> materialUboCreateInfo = {};
        materialUboCreateInfo.rhi = rhi;
        materialUboCreateInfo.commandQueue = commandQueue;
        materialUboCreateInfo.initialData = {spriteColorMaterialCreateInfo.spriteColorMaterialDataSource->color};

        m_materialUBO = std::make_unique<UniformBuffer<SpriteColorDescriptors::Material>>(materialUboCreateInfo);
    }
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

void SpriteColorMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) {
    m_descriptorSet->set(SpriteColorDescriptors::uCamera, params.globalDescriptors->camera_ubo()->descriptor());
    m_descriptorSet->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

}
