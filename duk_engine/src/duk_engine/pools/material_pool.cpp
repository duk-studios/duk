//
// Created by rov on 11/21/2023.
//

#include <duk_engine/pools/material_pool.h>

namespace duk::engine {

MaterialPool::MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo) :
    m_renderer(materialPoolCreateInfo.renderer),
    m_imagePool(materialPoolCreateInfo.imagePool) {

}

Resource<duk::renderer::PhongMaterial> MaterialPool::create_phong_material() {
    duk::renderer::PhongMaterialCreateInfo phongMaterialCreateInfo = {};
    phongMaterialCreateInfo.renderer = m_renderer;

    const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    auto material = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
    material->update_base_color({1.0f, 1.0f, 1.0f});
    material->update_shininess(32);
    material->update_base_color_image(m_imagePool->white_image().get(), sampler);
    material->update_shininess_image(m_imagePool->white_image().get(), sampler);
    return insert(material).as<duk::renderer::PhongMaterial>();
}

Resource<duk::renderer::ColorMaterial> MaterialPool::create_color_material() {
    duk::renderer::ColorMaterialCreateInfo colorMaterialCreateInfo = {};
    colorMaterialCreateInfo.renderer = m_renderer;

    auto material = std::make_shared<duk::renderer::ColorMaterial>(colorMaterialCreateInfo);
    material->set_color({1.0f, 1.0f, 1.0f, 1.0f});
    return insert(material).as<duk::renderer::ColorMaterial>();
}

Resource<duk::renderer::FullscreenMaterial> MaterialPool::create_fullscreen_material() {
    duk::renderer::FullscreenMaterialCreateInfo fullscreenMaterialCreateInfo = {};
    fullscreenMaterialCreateInfo.renderer = m_renderer;

    const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    auto material = std::make_shared<duk::renderer::FullscreenMaterial>(fullscreenMaterialCreateInfo);
    material->update(m_imagePool->white_image().get(), sampler);
    return insert(material).as<duk::renderer::FullscreenMaterial>();
}

}
