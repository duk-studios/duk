/// 12/09/2023
/// color_material.h

#ifndef DUK_RENDERER_COLOR_MATERIAL_H
#define DUK_RENDERER_COLOR_MATERIAL_H

#include <duk_renderer/resources/materials/color/color_descriptors.h>
#include <duk_renderer/resources/materials/instance_buffer.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/storage_buffer.h>
#include <duk_renderer/resources/materials/uniform_buffer.h>

namespace duk::renderer {

class ColorMaterialDataSource : public MaterialDataSource {
public:
    ColorMaterialDataSource();

    const rhi::ShaderDataSource* shader_data_source() const override;

    std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const override;

    glm::vec4 color;
    Texture colorTexture;

protected:
    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;
};

struct ColorInstanceBufferCreateInfo {
    Renderer* renderer;
};

class ColorInstanceBuffer : public InstanceBuffer {
public:
    ColorInstanceBuffer(const ColorInstanceBufferCreateInfo& phongInstanceBufferCreateInfo);

    void insert(const scene::Object& object) override;

    void clear() override;

    void flush() override;

    StorageBuffer<ColorDescriptors::Instance>* transform_buffer();

private:
    std::unique_ptr<StorageBuffer<ColorDescriptors::Instance>> m_transformSBO;
};

struct ColorMaterialDescriptorSetCreateInfo {
    Renderer* renderer;
    const ColorMaterialDataSource* colorMaterialDataSource;
};

class ColorMaterialDescriptorSet : public MaterialDescriptorSet {
public:
    explicit ColorMaterialDescriptorSet(const ColorMaterialDescriptorSetCreateInfo& colorMaterialDescriptorSetCreateInfo);

    void set_color(const glm::vec4& color);

    void set_color_texture(const Texture& colorTexture);

    void update(const DrawParams& params) override;

    void bind(duk::rhi::CommandBuffer* commandBuffer) override;

    uint32_t size() const override;

    ImageResource& image_at(uint32_t index) override;

    bool is_image(uint32_t index) override;

    InstanceBuffer* instance_buffer() override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<ColorInstanceBuffer> m_instanceBuffer;
    std::unique_ptr<UniformBuffer<ColorDescriptors::Material>> m_materialUBO;
    Texture m_colorTexture;
};

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::ColorMaterialDataSource& materialDataSource) {
    visitor->visit_member(materialDataSource.color, MemberDescription("color"));
    visitor->visit_member_object(materialDataSource.colorTexture, MemberDescription("colorTexture"));
}

}// namespace duk::serial

#endif// DUK_RENDERER_COLOR_MATERIAL_H
