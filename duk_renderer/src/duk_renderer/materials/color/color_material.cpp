/// 12/09/2023
/// color_material.cpp

#include <duk_renderer/materials/color/color_material.h>
#include <duk_renderer/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

static const ColorShaderDataSource kColorShaderDataSource;

ColorMaterialDataSource::ColorMaterialDataSource() :
    MaterialDataSource(MaterialType::COLOR),
    color() {

}

duk::hash::Hash ColorMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, color);
    return hash;
}

ColorMaterial::ColorMaterial(const ColorMaterialCreateInfo& colorMaterialCreateInfo) :
    Material(colorMaterialCreateInfo.renderer, &kColorShaderDataSource),
    m_descriptorSet({colorMaterialCreateInfo.renderer->rhi(), &kColorShaderDataSource}){

    auto rhi = colorMaterialCreateInfo.renderer->rhi();
    auto commandQueue = colorMaterialCreateInfo.renderer->main_command_queue();

    {
        StorageBufferCreateInfo instanceSBOCreateInfo = {};
        instanceSBOCreateInfo.rhi = rhi;
        instanceSBOCreateInfo.commandQueue = commandQueue;
        instanceSBOCreateInfo.initialSize = 1;
        m_instanceSBO = std::make_unique<color::InstanceSBO>(instanceSBOCreateInfo);
    }

    {
        UniformBufferCreateInfo<color::Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData.color = colorMaterialCreateInfo.colorMaterialDataSource->color;
        m_materialUBO = std::make_unique<color::MaterialUBO>(materialUBOCreateInfo);
    }

    m_descriptorSet.set(ColorDescriptorSet::Bindings::uInstances, *m_instanceSBO);
    m_descriptorSet.set(ColorDescriptorSet::Bindings::uMaterial, *m_materialUBO);
}

void ColorMaterial::set_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void ColorMaterial::clear_instances() {
    m_instanceSBO->clear();
}

void ColorMaterial::insert_instance(const Material::InsertInstanceParams& params) {
    auto& transform = m_instanceSBO->next();
    transform.model = duk::renderer::model_matrix_3d(params.object);
}

void ColorMaterial::flush_instances() {
    m_instanceSBO->flush();
}

void ColorMaterial::apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) {
    // updates current camera UBO
    m_descriptorSet.set(ColorDescriptorSet::Bindings::uCamera, *params.globalDescriptors->camera_ubo());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet.flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.handle(), 0);
}

}