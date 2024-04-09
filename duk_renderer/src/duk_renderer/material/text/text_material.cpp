//
// Created by Ricardo on 05/04/2024.
//
#include <duk_renderer/material/text/text_material.h>
#include <duk_renderer/material/text/text_shader_data_source.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/canvas.h>

namespace duk::renderer {

namespace detail {

static const TextShaderDataSource kColorShaderDataSource;

}

TextMaterialDataSource::TextMaterialDataSource()
    : color(glm::vec4(1))
    , atlasTexture() {

}

const rhi::ShaderDataSource* TextMaterialDataSource::shader_data_source() const {
    return &detail::kColorShaderDataSource;
}

std::unique_ptr<MaterialDescriptorSet> TextMaterialDataSource::create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const {
    TextMaterialDescriptorSetCreateInfo textMaterialDescriptorSetCreateInfo = {};
    textMaterialDescriptorSetCreateInfo.renderer = materialDescriptorSetCreateInfo.renderer;
    textMaterialDescriptorSetCreateInfo.textMaterialDataSource = this;
    return std::make_unique<TextMaterialDescriptorSet>(textMaterialDescriptorSetCreateInfo);
}

duk::hash::Hash TextMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, color);
    return hash;
}

TextMaterialDescriptorSet::TextMaterialDescriptorSet(const TextMaterialDescriptorSetCreateInfo& textMaterialDescriptorSetCreateInfo)
    : m_useCanvas(true) {
    auto renderer = textMaterialDescriptorSetCreateInfo.renderer;
    auto materialDataSource = textMaterialDescriptorSetCreateInfo.textMaterialDataSource;
    auto shaderDataSource = materialDataSource->shader_data_source();
    auto rhi = renderer->rhi();
    auto commandQueue = renderer->main_command_queue();

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = shaderDataSource->descriptor_set_descriptions().at(0);
        m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);
    }
    {
        UniformBufferCreateInfo<TextDescriptors::Transform> transformUBOCreateInfo = {};
        transformUBOCreateInfo.rhi = rhi;
        transformUBOCreateInfo.commandQueue = commandQueue;
        transformUBOCreateInfo.initialData.model = glm::mat4(1);
        m_instanceUBO = std::make_unique<UniformBuffer<TextDescriptors::Transform>>(transformUBOCreateInfo);
        m_descriptorSet->set(TextDescriptors::uInstance, m_instanceUBO->descriptor());
    }
    {
        UniformBufferCreateInfo<TextDescriptors::Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData.color = materialDataSource->color;
        m_materialUBO = std::make_unique<UniformBuffer<TextDescriptors::Material>>(materialUBOCreateInfo);
        m_descriptorSet->set(TextDescriptors::uMaterial, m_materialUBO->descriptor());
    }
    set_color(materialDataSource->color);
    set_atlas_texture(materialDataSource->atlasTexture);
}

void TextMaterialDescriptorSet::set_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void TextMaterialDescriptorSet::set_atlas_texture(const Texture& atlasTexture) {
    m_atlasTexture = atlasTexture;
}

void TextMaterialDescriptorSet::set_instance(const objects::Object& object) {
    auto [transform, canvasTransform] = object.components<Transform, CanvasTransform>();
    auto& model = m_instanceUBO->data().model;
    if (transform) {
        model = transform->model;
        m_useCanvas = false;
    }
    else if (canvasTransform) {
        model = canvasTransform->model;
        m_useCanvas = true;
    }
    else {
        model = glm::mat4(1);
    }
    m_instanceUBO->flush();
}

void TextMaterialDescriptorSet::update(const DrawParams& params) {
    // updates current camera UBO
    if (m_useCanvas) {
        m_descriptorSet->set(TextDescriptors::uCamera, params.globalDescriptors->canvas_ubo()->descriptor());
    }
    else {
        m_descriptorSet->set(TextDescriptors::uCamera, params.globalDescriptors->camera_ubo()->descriptor());
    }

    // these might have changed
    m_descriptorSet->set(TextDescriptors::uBaseColor, m_atlasTexture.descriptor());
    m_descriptorSet->set(TextDescriptors::uInstance, m_instanceUBO->descriptor());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet->flush();
}

void TextMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

uint32_t TextMaterialDescriptorSet::size() const {
    return TextDescriptors::kDescriptorCount;
}

}