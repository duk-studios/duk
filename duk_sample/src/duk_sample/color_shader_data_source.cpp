/// 16/06/2023
/// color_shader_data_source.cpp

#include <duk_sample/color_shader_data_source.h>
#include <duk_sample/application.h>

namespace duk::sample {

ColorShaderDataSource::ColorShaderDataSource() {

    insert_spir_v_code(duk::renderer::Shader::Module::VERTEX, Application::load_bytes("triangle.vert.spv"));
    insert_spir_v_code(duk::renderer::Shader::Module::FRAGMENT, Application::load_bytes("triangle.frag.spv"));

    insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC2);
    insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC4);
    insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC2);

    duk::renderer::DescriptorSetDescription descriptorSetDescription;
    auto& transformBinding = descriptorSetDescription.bindings.emplace_back();
    transformBinding.type = renderer::DescriptorType::UNIFORM_BUFFER;
    transformBinding.moduleMask = duk::renderer::Shader::Module::VERTEX;

    auto& uniformBinding = descriptorSetDescription.bindings.emplace_back();
    uniformBinding.type = duk::renderer::DescriptorType::UNIFORM_BUFFER;
    uniformBinding.moduleMask = duk::renderer::Shader::Module::FRAGMENT;

    auto& samplerBinding = descriptorSetDescription.bindings.emplace_back();
    samplerBinding.type = duk::renderer::DescriptorType::IMAGE_SAMPLER;
    samplerBinding.moduleMask = duk::renderer::Shader::Module::FRAGMENT;

    insert_descriptor_set_description(descriptorSetDescription);

    update_hash();
}

}

