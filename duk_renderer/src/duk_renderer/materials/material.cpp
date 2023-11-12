/// 12/09/2023
/// material.cpp

#include <duk_renderer/materials/material.h>
#include <duk_renderer/brushes/brush.h>

namespace duk::renderer {

Material::Material(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource) :
    m_painter({.renderer = renderer, .shaderDataSource = shaderDataSource}) {
}

void Material::clear_instances() {

}

void Material::insert_instance(const Material::InsertInstanceParams& params) {

}

void Material::flush_instances() {

}

void Material::paint(duk::rhi::CommandBuffer* commandBuffer, const PaintParams& params) {
    m_painter.use(commandBuffer, params);

    ApplyParams applyParams = {};
    applyParams.globalDescriptors = params.globalDescriptors;

    apply(commandBuffer, applyParams);

    params.brush->draw(commandBuffer, params.instanceCount, params.firstInstance);
}

Painter* Material::painter() {
    return &m_painter;
}

const Painter* Material::painter() const {
    return &m_painter;
}

}