/// 12/09/2023
/// material.cpp

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/brushes/brush.h>

namespace duk::renderer {

MaterialDataSource::MaterialDataSource(MaterialType type) :
        m_type(type) {

}

MaterialType MaterialDataSource::type() const {
    return m_type;
}

Material::Material(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource) :
    m_pipeline({.renderer = renderer, .shaderDataSource = shaderDataSource}) {
}

Material::~Material() = default;

Pipeline* Material::pipeline() {
    return &m_pipeline;
}

const Pipeline* Material::pipeline() const {
    return &m_pipeline;
}

}