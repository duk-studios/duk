//
// Created by Ricardo on 25/01/2024.
//

#ifndef DUK_RENDERER_SPRITE_COLOR_MATERIAL_H
#define DUK_RENDERER_SPRITE_COLOR_MATERIAL_H

#include <cstdint>
#include <duk_renderer/material/material.h>
#include <duk_renderer/material/sprites/sprite_color/sprite_color_descriptors.h>
#include <duk_renderer/material/sprites/sprite_color/sprite_color_shader_data_source.h>
#include <duk_renderer/material/uniform_buffer.h>
#include <duk_renderer/material/storage_buffer.h>

namespace duk::renderer {

class SpriteColorMaterialDataSource : public MaterialDataSource {
public:
    SpriteColorMaterialDataSource();

    const rhi::ShaderDataSource* shader_data_source() const override;

    std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const override;

    glm::vec4 color;
    ImageResource image;

protected:
    hash::Hash calculate_hash() const override;
};

struct SpriteColorInstanceBufferCreateInfo {
    Renderer* renderer;
};

class SpriteColorInstanceBuffer : public InstanceBuffer {
public:
    explicit SpriteColorInstanceBuffer(const SpriteColorInstanceBufferCreateInfo& spriteColorInstanceBufferCreateInfo);

    void insert(const duk::objects::Object& object) override;

    void clear() override;

    void flush() override;

    const StorageBuffer<SpriteColorDescriptors::Instance>* instances() const;

private:
    std::unique_ptr<StorageBuffer<SpriteColorDescriptors::Instance>> m_instancesSBO;
};

struct SpriteColorMaterialDescriptorSetCreateInfo {
    Renderer* renderer;
    const SpriteColorMaterialDataSource* spriteColorMaterialDataSource;
};

class SpriteColorMaterialDescriptorSet : public MaterialDescriptorSet {
public:
    explicit SpriteColorMaterialDescriptorSet(const SpriteColorMaterialDescriptorSetCreateInfo& spriteColorMaterialCreateInfo);

    void set_color(const glm::vec4& color);

    uint32_t size() const override;

    ImageResource& image_at(uint32_t index) override;

    bool is_image(uint32_t index) override;

    DUK_NO_DISCARD InstanceBuffer* instance_buffer() override;

    void update(const DrawParams& params) override;

    void bind(duk::rhi::CommandBuffer* commandBuffer) override;

private:
    std::unique_ptr<UniformBuffer<SpriteColorDescriptors::Material>> m_materialUBO;
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<SpriteColorInstanceBuffer> m_instanceBuffer;
    ImageResource m_image;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_COLOR_MATERIAL_H
