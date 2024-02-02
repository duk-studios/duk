//
// Created by rov on 1/19/2024.
//
#include <duk_renderer/resources/materials/sprite_material.h>

namespace duk::renderer {

SpriteMaterial::SpriteMaterial(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource) :
    Material(renderer, shaderDataSource){

}

}
