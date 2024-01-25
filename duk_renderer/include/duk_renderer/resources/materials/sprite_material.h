//
// Created by rov on 1/19/2024.
//

#ifndef DUK_SPRITE_MATERIAL_H
#define DUK_SPRITE_MATERIAL_H

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/sprite.h>

namespace duk::renderer {

class SpriteMaterial : public Material {
protected:

    SpriteMaterial(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

public:

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params, uint32_t spriteHandle) = 0;

    struct PushSpriteParams {
        duk::scene::Object object;
        Sprite* sprite = nullptr;
    };

    DUK_NO_DISCARD uint32_t push_sprite(const PushSpriteParams& params);

    void clear_sprites();

protected:

    virtual uint32_t calculate_sprite_handle(const PushSpriteParams& params);

    virtual std::shared_ptr<duk::rhi::DescriptorSet> allocate_descriptor_set(const PushSpriteParams& params) = 0;

    virtual void update_descriptor_set(duk::rhi::DescriptorSet* descriptorSet, const PushSpriteParams& params) = 0;

private:
    std::unordered_map<uint32_t, size_t> m_handleToDescriptorSetIndex;
    std::vector<std::shared_ptr<duk::rhi::DescriptorSet>> m_descriptorSets;
    size_t m_usedDescriptorSets;
};

using SpriteMaterialResource = duk::pool::Resource<SpriteMaterial>;

}

#endif //DUK_SPRITE_MATERIAL_H
