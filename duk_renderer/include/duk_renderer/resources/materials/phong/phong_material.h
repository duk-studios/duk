/// 13/10/2023
/// phong_material.h

#ifndef DUK_RENDERER_PHONG_MATERIAL_H
#define DUK_RENDERER_PHONG_MATERIAL_H

#include <duk_renderer/resources/materials/instance_buffer.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/phong/phong_descriptors.h>
#include <duk_renderer/resources/materials/phong/phong_shader_data_source.h>
#include <duk_renderer/resources/materials/storage_buffer.h>
#include <duk_renderer/resources/materials/uniform_buffer.h>

namespace duk::renderer {

class PhongMaterialDataSource : public MaterialDataSource {
public:
    PhongMaterialDataSource();

    const rhi::ShaderDataSource* shader_data_source() const override;

    std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const override;

public:
    // for easy access, these are public
    Texture albedoTexture;
    glm::vec4 albedo;
    Texture specularTexture;
    float specular;

protected:
    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;
};

struct PhongInstanceBufferCreateInfo {
    Renderer* renderer;
};

class PhongInstanceBuffer : public InstanceBuffer {
public:
    PhongInstanceBuffer(const PhongInstanceBufferCreateInfo& phongInstanceBufferCreateInfo);

    void insert(const scene::Object& object) override;

    void clear() override;

    void flush() override;

    StorageBuffer<PhongDescriptors::Transform>* transform_buffer();

private:
    std::unique_ptr<StorageBuffer<PhongDescriptors::Transform>> m_transformSBO;
};

struct PhongMaterialDescriptorSetCreateInfo {
    Renderer* renderer;
    const PhongMaterialDataSource* phongMaterialDataSource;
};

class PhongMaterialDescriptorSet : public MaterialDescriptorSet {
public:
    PhongMaterialDescriptorSet(const PhongMaterialDescriptorSetCreateInfo& phongMaterialDescriptorSetCreateInfo);

    ~PhongMaterialDescriptorSet() override = default;

    void set_albedo_color(const glm::vec4& color);

    void set_albedo_texture(const Texture& albedo);

    void set_specular_value(float value);

    void set_specular_texture(const Texture& specular);

    uint32_t size() const override;

    ImageResource& image_at(uint32_t index) override;

    bool is_image(uint32_t index) override;

    DUK_NO_DISCARD InstanceBuffer* instance_buffer() override;

    void bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& drawParams) override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<PhongInstanceBuffer> m_instanceBuffer;
    std::unique_ptr<UniformBuffer<PhongDescriptors::Material>> m_materialUBO;
    Texture m_albedo;
    Texture m_specular;
};

}// namespace duk::renderer

namespace duk::json {

template<>
inline void from_json<duk::renderer::PhongMaterialDataSource>(const rapidjson::Value& jsonObject, duk::renderer::PhongMaterialDataSource& phongMaterialDataSource) {
    phongMaterialDataSource.albedo = from_json_member<glm::vec4>(jsonObject, "albedo", glm::vec4(1));
    phongMaterialDataSource.albedoTexture = from_json_member<duk::renderer::Texture>(jsonObject, "albedo-texture");
    phongMaterialDataSource.specular = from_json_member<float>(jsonObject, "specular", 1.0f);
    phongMaterialDataSource.specularTexture = from_json_member<duk::renderer::Texture>(jsonObject, "specular-texture");
}

}// namespace duk::json

#endif// DUK_RENDERER_PHONG_MATERIAL_H
