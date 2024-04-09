//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_RENDERER_TEXT_MATERIAL_H
#define DUK_RENDERER_TEXT_MATERIAL_H

#include <duk_renderer/material/material.h>
#include <duk_renderer/material/storage_buffer.h>
#include <duk_renderer/material/text/text_descriptors.h>
#include <duk_renderer/material/uniform_buffer.h>

namespace duk::renderer {

class TextMaterialDataSource : public MaterialDataSource {
public:
    TextMaterialDataSource();

    const rhi::ShaderDataSource* shader_data_source() const override;

    std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const override;

    glm::vec4 color;
    Texture atlasTexture;

protected:
    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;
};

struct TextMaterialDescriptorSetCreateInfo {
    Renderer* renderer;
    const TextMaterialDataSource* textMaterialDataSource;
};

class TextMaterialDescriptorSet : public MaterialDescriptorSet {
public:
    explicit TextMaterialDescriptorSet(const TextMaterialDescriptorSetCreateInfo& colorMaterialDescriptorSetCreateInfo);

    void set_color(const glm::vec4& color);

    void set_atlas_texture(const Texture& atlasTexture);

    void set_instance(const duk::objects::Object& object);

    void update(const DrawParams& params) override;

    void bind(duk::rhi::CommandBuffer* commandBuffer) override;

    uint32_t size() const override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<UniformBuffer<TextDescriptors::Transform>> m_instanceUBO;
    std::unique_ptr<UniformBuffer<TextDescriptors::Material>> m_materialUBO;
    Texture m_atlasTexture;
    bool m_useCanvas;
};

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::TextMaterialDataSource& materialDataSource) {
    visitor->visit_member(materialDataSource.color, MemberDescription("color"));
}

}// namespace duk::serial

#endif//DUK_RENDERER_TEXT_MATERIAL_H
