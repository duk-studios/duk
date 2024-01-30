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

MaterialResource MaterialPool::create(duk::pool::ResourceId resourceId, const MaterialDataSource* materialDataSource) {
    switch (materialDataSource->type()) {
        case MaterialType::COLOR: return create_color(resourceId, materialDataSource->as<ColorMaterialDataSource>());
        case MaterialType::PHONG: return create_phong(resourceId, materialDataSource->as<PhongMaterialDataSource>());
        default: throw std::invalid_argument("invalid MaterialType to create");
    }
}

PhongMaterialResource MaterialPool::create_phong(duk::pool::ResourceId resourceId) {

    const duk::rhi::Sampler kSampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    PhongMaterialDataSource phongMaterialDataSource = {};
    phongMaterialDataSource.baseColorImage = m_imagePool->white_image();
    phongMaterialDataSource.baseColor = glm::vec4(1.0f);
    phongMaterialDataSource.baseColorSampler = kSampler;
    phongMaterialDataSource.shininess = 32.0f;
    phongMaterialDataSource.shininessImage = m_imagePool->white_image();
    phongMaterialDataSource.shininessSampler = kSampler;

    return create_phong(resourceId, &phongMaterialDataSource);
}

PhongMaterialResource MaterialPool::create_phong(duk::pool::ResourceId resourceId, const PhongMaterialDataSource* phongMaterialDataSource) {
    PhongMaterialCreateInfo phongMaterialCreateInfo = {};
    phongMaterialCreateInfo.renderer = m_renderer;
    phongMaterialCreateInfo.phongMaterialDataSource = phongMaterialDataSource;

    return insert(resourceId, std::make_shared<PhongMaterial>(phongMaterialCreateInfo)).as<PhongMaterial>();
}

ColorMaterialResource MaterialPool::create_color(duk::pool::ResourceId resourceId) {
    ColorMaterialDataSource colorMaterialDataSource = {};
    colorMaterialDataSource.color = glm::vec4(1.0f);
    return create_color(resourceId, &colorMaterialDataSource);
}

ColorMaterialResource MaterialPool::create_color(duk::pool::ResourceId resourceId, const ColorMaterialDataSource* colorMaterialDataSource) {
    ColorMaterialCreateInfo colorMaterialCreateInfo = {};
    colorMaterialCreateInfo.renderer = m_renderer;
    colorMaterialCreateInfo.colorMaterialDataSource = colorMaterialDataSource;

    return insert(resourceId, std::make_shared<ColorMaterial>(colorMaterialCreateInfo)).as<ColorMaterial>();
}

FullscreenMaterialResource MaterialPool::create_fullscreen(duk::pool::ResourceId resourceId) {
    FullscreenMaterialCreateInfo fullscreenMaterialCreateInfo = {};
    fullscreenMaterialCreateInfo.renderer = m_renderer;

    const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

    auto material = std::make_shared<FullscreenMaterial>(fullscreenMaterialCreateInfo);
    material->update(m_imagePool->white_image().get(), sampler);
    return insert(resourceId, material).as<FullscreenMaterial>();
}

SpriteColorMaterialResource MaterialPool::create_sprite_color(duk::pool::ResourceId resourceId) {
    SpriteColorMaterialDataSource spriteColorMaterialDataSource = {};
    spriteColorMaterialDataSource.color = glm::vec4(1);
    return create_sprite_color(resourceId, &spriteColorMaterialDataSource);
}

SpriteColorMaterialResource MaterialPool::create_sprite_color(duk::pool::ResourceId resourceId, const SpriteColorMaterialDataSource* spriteColorMaterialDataSource) {
    SpriteColorMaterialCreateInfo spriteColorMaterialCreateInfo = {};
    spriteColorMaterialCreateInfo.renderer = m_renderer;
    spriteColorMaterialCreateInfo.spriteColorMaterialDataSource = spriteColorMaterialDataSource;

    return insert(resourceId, std::make_shared<SpriteColorMaterial>(spriteColorMaterialCreateInfo)).as<SpriteColorMaterial>();
}

}
