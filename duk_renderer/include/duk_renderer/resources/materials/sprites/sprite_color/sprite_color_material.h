//
// Created by Ricardo on 25/01/2024.
//

#ifndef DUK_RENDERER_SPRITE_COLOR_MATERIAL_H
#define DUK_RENDERER_SPRITE_COLOR_MATERIAL_H

#include <duk_renderer/resources/materials/sprite_material.h>
#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_types.h>
#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_shader_data_source.h>
#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_descriptor_sets.h>

namespace duk::renderer {

class SpriteColorMaterialDataSource : public MaterialDataSource {
public:

    SpriteColorMaterialDataSource();

    glm::vec4 color;

protected:
    hash::Hash calculate_hash() const override;
};


struct SpriteColorMaterialCreateInfo {
    Renderer* renderer;
    const SpriteColorMaterialDataSource* spriteColorMaterialDataSource;
};

class SpriteColorMaterial : public SpriteMaterial {
public:
    explicit SpriteColorMaterial(const SpriteColorMaterialCreateInfo& spriteColorMaterialCreateInfo);

    void set_color(const glm::vec4& color);

    DUK_NO_DISCARD uint32_t get_sub_material_index(const PushSpriteParams& params) override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params, uint32_t subMaterialIndex) override;

    void clear_sub_materials() override;

protected:

    std::unique_ptr<SpriteColorDescriptorSet> allocate_descriptor_set(const PushSpriteParams& params);

    void update_sub_material(SpriteColorDescriptorSet* descriptorSet, const SpriteMaterial::PushSpriteParams& params);

private:
    Renderer* m_renderer;
    std::unique_ptr<sprite_color::MaterialUBO> m_materialUBO;
    std::unordered_map<uint32_t, size_t> m_handleToDescriptorSetIndex;
    std::vector<std::unique_ptr<SpriteColorDescriptorSet>> m_descriptorSets;
    size_t m_usedDescriptorSets;
};

using SpriteColorMaterialResource = duk::resource::ResourceT<SpriteColorMaterial>;

}

#endif //DUK_RENDERER_SPRITE_COLOR_MATERIAL_H
