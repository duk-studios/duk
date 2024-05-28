//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/material/material_pool.h>

namespace duk::renderer {

MaterialPool::MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo)
    : m_renderer(materialPoolCreateInfo.renderer) {
}

MaterialResource MaterialPool::create(duk::resource::Id resourceId, MaterialData materialData) {
    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.renderer = m_renderer;
    materialCreateInfo.materialData = std::move(materialData);
    return insert(resourceId, std::make_shared<Material>(materialCreateInfo));
}

}// namespace duk::renderer
