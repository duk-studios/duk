/// 12/09/2023
/// color_material.cpp

#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_material.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

namespace detail {

static const SpriteColorShaderDataSource kSpriteColorShaderDataSource;

static uint32_t calculate_sub_material_index(const SpriteMaterial::PushSpriteParams& params) {
    return static_cast<uint32_t>(params.sprite->image()->hash());
}

}

SpriteColorMaterialDataSource::SpriteColorMaterialDataSource() :
    MaterialDataSource(MaterialType::SPRITE_COLOR) {

}

hash::Hash SpriteColorMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, color);
    return hash;
}

SpriteColorMaterial::SpriteColorMaterial(const SpriteColorMaterialCreateInfo& spriteColorMaterialCreateInfo) :
    SpriteMaterial(spriteColorMaterialCreateInfo.renderer, &detail::kSpriteColorShaderDataSource),
    m_renderer(spriteColorMaterialCreateInfo.renderer),
    m_usedDescriptorSets(0) {

    {
        UniformBufferCreateInfo<sprite_color::Material> materialUboCreateInfo = {};
        materialUboCreateInfo.rhi = m_renderer->rhi();
        materialUboCreateInfo.commandQueue = m_renderer->main_command_queue();
        materialUboCreateInfo.initialData = {spriteColorMaterialCreateInfo.spriteColorMaterialDataSource->color};

        m_materialUBO = std::make_unique<sprite_color::MaterialUBO>(materialUboCreateInfo);
    }
}

void SpriteColorMaterial::set_color(const glm::vec4& color) {
    auto& material = m_materialUBO->data();
    material.color = color;
    m_materialUBO->flush();
}

void SpriteColorMaterial::apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params, uint32_t subMaterialIndex) {
    pipeline()->use(commandBuffer, params);

    auto& descriptorSet = m_descriptorSets.at(subMaterialIndex);
    descriptorSet->set(SpriteColorDescriptorSet::Bindings::uCamera, duk::rhi::Descriptor::uniform_buffer(params.globalDescriptors->camera_ubo()->buffer()));
    descriptorSet->flush();

    commandBuffer->bind_descriptor_set(descriptorSet->handle(), 0);
}

std::unique_ptr<SpriteColorDescriptorSet> SpriteColorMaterial::allocate_descriptor_set(const SpriteMaterial::PushSpriteParams& params) {

    SpriteColorDescriptorSetCreateInfo spriteColorDescriptorSetCreateInfo = {};
    spriteColorDescriptorSetCreateInfo.shaderDataSource = &detail::kSpriteColorShaderDataSource;
    spriteColorDescriptorSetCreateInfo.rhi = m_renderer->rhi();

    auto descriptorSet = std::make_unique<SpriteColorDescriptorSet>(spriteColorDescriptorSetCreateInfo);

    descriptorSet->set(SpriteColorDescriptorSet::Bindings::uMaterial, duk::rhi::Descriptor::uniform_buffer(m_materialUBO->buffer()));

    update_sub_material(descriptorSet.get(), params);

    return descriptorSet;
}

void SpriteColorMaterial::update_sub_material(SpriteColorDescriptorSet* descriptorSet, const SpriteMaterial::PushSpriteParams& params) {
    static const duk::rhi::Sampler kSampler = {duk::rhi::Sampler::Filter::NEAREST, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE};
    descriptorSet->set(SpriteColorDescriptorSet::Bindings::uBaseColor, duk::rhi::Descriptor::image_sampler(params.sprite->image().get(), duk::rhi::Image::Layout::SHADER_READ_ONLY, kSampler));
}

uint32_t SpriteColorMaterial::get_sub_material_index(const SpriteMaterial::PushSpriteParams& params) {
    auto handle = detail::calculate_sub_material_index(params);

    auto it = m_handleToDescriptorSetIndex.find(handle);

    if (it != m_handleToDescriptorSetIndex.end()) {
        // already allocated
        return it->second;
    }

    auto descriptorSetIndex = m_usedDescriptorSets++;

    if (descriptorSetIndex < m_descriptorSets.size()) {
        // we have a free descriptor set for use, just update it
        update_sub_material(m_descriptorSets.at(descriptorSetIndex).get(), params);
    }
    else {
        // allocate a new descriptor
        m_descriptorSets.emplace_back(allocate_descriptor_set(params));
    }
    m_handleToDescriptorSetIndex.emplace(handle, descriptorSetIndex);

    return descriptorSetIndex;
}

void SpriteColorMaterial::clear_sub_materials() {
    m_usedDescriptorSets = 0;
    m_handleToDescriptorSetIndex.clear();
}

}