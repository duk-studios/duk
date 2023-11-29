//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/pools/material_pool.h>
#include <duk_renderer/pools/image_pool.h>

namespace duk::renderer {

MaterialPool::MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo) :
    m_renderer(materialPoolCreateInfo.renderer),
    m_imagePool(materialPoolCreateInfo.imagePool) {

}

PhongMaterialResource MaterialPool::create_phong_material() {
    PhongMaterialCreateInfo phongMaterialCreateInfo = {};
    phongMaterialCreateInfo.renderer = m_renderer;

    const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    auto material = std::make_shared<PhongMaterial>(phongMaterialCreateInfo);
    material->update_base_color({1.0f, 1.0f, 1.0f});
    material->update_shininess(32);
    material->update_base_color_image(m_imagePool->white_image().get(), sampler);
    material->update_shininess_image(m_imagePool->white_image().get(), sampler);
    return insert(material).as<PhongMaterial>();
}

ColorMaterialResource MaterialPool::create_color_material() {
    ColorMaterialCreateInfo colorMaterialCreateInfo = {};
    colorMaterialCreateInfo.renderer = m_renderer;

    auto material = std::make_shared<ColorMaterial>(colorMaterialCreateInfo);
    material->set_color({1.0f, 1.0f, 1.0f, 1.0f});
    return insert(material).as<ColorMaterial>();
}

FullscreenMaterialResource MaterialPool::create_fullscreen_material() {
    FullscreenMaterialCreateInfo fullscreenMaterialCreateInfo = {};
    fullscreenMaterialCreateInfo.renderer = m_renderer;

    const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    auto material = std::make_shared<FullscreenMaterial>(fullscreenMaterialCreateInfo);
    material->update(m_imagePool->white_image().get(), sampler);
    return insert(material).as<FullscreenMaterial>();
}

}
