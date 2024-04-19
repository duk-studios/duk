/// 12/09/2023
/// color_material.h

#ifndef DUK_RENDERER_COLOR_MATERIAL_H
#define DUK_RENDERER_COLOR_MATERIAL_H

#include <duk_renderer/material/color/color_descriptors.h>
#include <duk_renderer/material/instance_buffer.h>
#include <duk_renderer/material/material.h>
#include <duk_renderer/material/storage_buffer.h>
#include <duk_renderer/material/uniform_buffer.h>

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

    void insert(const objects::Object& object) override;

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

template<>
inline void from_json<duk::renderer::ColorMaterialDataSource>(const rapidjson::Value& json, duk::renderer::ColorMaterialDataSource& materialDataSource) {
    from_json_member(json, "color", materialDataSource.color);
    from_json_member(json, "texture", materialDataSource.colorTexture);
}

template<>
inline void to_json<duk::renderer::ColorMaterialDataSource>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::ColorMaterialDataSource& materialDataSource) {
    to_json_member(document, json, "color", materialDataSource.color);
    to_json_member(document, json, "texture", materialDataSource.colorTexture);
}

}// namespace duk::serial

#endif// DUK_RENDERER_COLOR_MATERIAL_H
