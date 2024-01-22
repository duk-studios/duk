//
// Created by rov on 1/19/2024.
//
#include <duk_renderer/resources/materials/sprite_material.h>

namespace duk::renderer {

SpriteMaterial::SpriteMaterial(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource) :
    Material(renderer, shaderDataSource),
    m_usedDescriptorSets(0) {

}

uint32_t SpriteMaterial::push_sprite(const SpriteMaterial::PushSpriteParams& params) {
    auto handle = calculate_sprite_handle(params);

    auto it = m_handleToDescriptorSetIndex.find(handle);

    if (it != m_handleToDescriptorSetIndex.end()) {
        // already allocated
        return handle;
    }

    auto descriptorSetIndex = m_usedDescriptorSets++;

    if (descriptorSetIndex < m_descriptorSets.size()) {
        // we have a free descriptor set for use, just update it
        update_descriptor_set(m_descriptorSets.at(descriptorSetIndex).get(), params);
    }
    else {
        // allocate a new descriptor
        m_descriptorSets.emplace_back(allocate_descriptor_set(params));
    }
    m_handleToDescriptorSetIndex.emplace(handle, descriptorSetIndex);

    return handle;
}

void SpriteMaterial::clear_sprites() {
    m_handleToDescriptorSetIndex.clear();
    m_usedDescriptorSets = 0;
}

uint32_t SpriteMaterial::calculate_sprite_handle(const SpriteMaterial::PushSpriteParams& params) {
    return static_cast<uint32_t>(params.sprite->image()->hash());
}

}
