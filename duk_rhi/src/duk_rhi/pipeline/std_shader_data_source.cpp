/// 16/05/2023
/// std_shader_data_source.cpp

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <cassert>

namespace duk::rhi {

StdShaderDataSource::StdShaderDataSource() : m_moduleMask(0) {
}

StdShaderDataSource::~StdShaderDataSource() = default;

Shader::Module::Mask StdShaderDataSource::module_mask() const {
    return m_moduleMask;
}

const std::vector<uint8_t>& StdShaderDataSource::shader_module_spir_v_code(Shader::Module::Bits type) const {
    assert(m_moduleMask & type && "Shader::Module was not set");
    const auto& module = m_shaderModules.at(type);
    return module.spirVCode;
}

const std::vector<DescriptorSetDescription>& StdShaderDataSource::descriptor_set_descriptions() const {
    return m_descriptorSetDescriptions;
}

duk::hash::Hash StdShaderDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;

    duk::hash::hash_combine(hash, m_moduleMask);
    for (auto& [type, module]: m_shaderModules) {
        duk::hash::hash_combine(hash, type);
        duk::hash::hash_combine(hash, module.spirVCode.data(), module.spirVCode.size());
    }

    for (auto& descriptorSetDescription: m_descriptorSetDescriptions) {
        duk::hash::hash_combine(hash, descriptorSetDescription.bindings.begin(), descriptorSetDescription.bindings.end());
    }

    duk::hash::hash_combine(hash, m_vertexLayout);

    return hash;
}

void StdShaderDataSource::insert_spir_v_code(Shader::Module::Bits type, const uint8_t* data, size_t size) {
    assert(!(m_moduleMask & type) && "Shader::Module was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode.insert(module.spirVCode.end(), data, data + size);
}

void StdShaderDataSource::insert_spir_v_code(Shader::Module::Bits type, const std::vector<uint8_t>& data) {
    assert(!(m_moduleMask & type) && "Shader::Module was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode = data;
}

void StdShaderDataSource::insert_spir_v_code(Shader::Module::Bits type, std::vector<uint8_t>&& data) {
    assert(!(m_moduleMask & type) && "Shader::Module was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode = std::move(data);
}

void StdShaderDataSource::insert_vertex_attribute(VertexInput::Format format) {
    m_vertexLayout.insert(format);
}

void StdShaderDataSource::insert_vertex_attributes(const std::initializer_list<VertexInput::Format>& formats) {
    m_vertexLayout.insert(formats);
}

void StdShaderDataSource::insert_descriptor_set_description(const DescriptorSetDescription& descriptorSetDescription) {
    m_descriptorSetDescriptions.push_back(descriptorSetDescription);
}

void StdShaderDataSource::insert_descriptor_set_description(DescriptorSetDescription&& descriptorSetDescription) {
    m_descriptorSetDescriptions.emplace_back(std::move(descriptorSetDescription));
}

const VertexLayout& StdShaderDataSource::vertex_layout() const {
    return m_vertexLayout;
}

}// namespace duk::rhi