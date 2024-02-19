//
// Created by Ricardo on 30/01/2024.
//

#include <duk_sample/sprite_system.h>
#include <duk_engine/engine.h>

#include <duk_renderer/pools/sprite_pool.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/transform.h>

namespace duk::sample {

void SpriteSystem::init() {

    auto engine = this->engine();
    auto scene = engine->scene();

    auto sprite = engine->renderer()->sprite_pool()->white_square();

    duk::renderer::SpriteColorMaterialDataSource materialDataSource = {};
    materialDataSource.color = glm::vec4(1);
    materialDataSource.image = sprite->image();

    // sprite testing
    auto spriteColorMaterial = engine->renderer()->material_pool()->create(duk::resource::Id(1001), &materialDataSource);

    auto object = scene->add_object();

    auto spriteRenderer = object.add<duk::renderer::SpriteRenderer>();
    spriteRenderer->material = spriteColorMaterial;
    spriteRenderer->sprite = sprite;

    auto scale = object.add<duk::renderer::Scale3D>();
    scale->value = glm::vec3(1000);
}

void SpriteSystem::update() {

}

}