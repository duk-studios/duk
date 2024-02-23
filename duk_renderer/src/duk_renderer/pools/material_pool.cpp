//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/pools/material_pool.h>

namespace duk::renderer {

MaterialPool::MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo)
    : m_renderer(materialPoolCreateInfo.renderer) {
}

MaterialResource MaterialPool::create(duk::resource::Id resourceId, const MaterialDataSource* materialDataSource) {
    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.renderer = m_renderer;
    materialCreateInfo.materialDataSource = materialDataSource;
    return insert(resourceId, std::make_shared<Material>(materialCreateInfo));
}

}// namespace duk::renderer
