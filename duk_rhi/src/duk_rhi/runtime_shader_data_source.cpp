/// 16/05/2023
/// runtime_shader_data_source.cpp

#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/shader_reflection.h>

#include <duk_hash/hash_combine.h>
#include <duk_macros/assert.h>

#include <stdexcept>

namespace duk::rhi {

RuntimeShaderDataSource::RuntimeShaderDataSource(const RuntimeShaderDataSourceCreateInfo& createInfo)
    : m_moduleMask(0) {
    // Registers one stage if the provided bytecode is non-empty.
    auto addStage = [&](ShaderModule::Bits stage, std::vector<uint8_t> code) {
        if (code.empty()) {
            return;
        }
        m_moduleMask |= stage;
        m_shaderModules[stage] = std::move(code);
    };

    addStage(ShaderModule::VERTEX, createInfo.vertexShaderCode);
    addStage(ShaderModule::TESSELLATION_CONTROL, createInfo.tessellationControlShaderCode);
    addStage(ShaderModule::TESSELLATION_EVALUATION, createInfo.tessellationEvaluationShaderCode);
    addStage(ShaderModule::GEOMETRY, createInfo.geometryShaderCode);
    addStage(ShaderModule::FRAGMENT, createInfo.fragmentShaderCode);
    addStage(ShaderModule::COMPUTE, createInfo.computeShaderCode);

    DUK_ASSERT(m_moduleMask != 0 && "StdShaderDataSource: no shader stages provided");

    ShaderReflection reflection(m_shaderModules);
    m_bindingLayout = reflection.binding_layout();
    m_vertexLayout = reflection.vertex_layout();

    update_hash();
}

RuntimeShaderDataSource::~RuntimeShaderDataSource() = default;

ShaderModule::Mask RuntimeShaderDataSource::module_mask() const {
    return m_moduleMask;
}

const std::vector<uint8_t>& RuntimeShaderDataSource::shader_module_spir_v_code(ShaderModule::Bits type) const {
    DUK_ASSERT(m_moduleMask & type && "ShaderModule stage was not provided");
    return m_shaderModules.at(type);
}

const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& RuntimeShaderDataSource::shader_modules() const {
    return m_shaderModules;
}

const BindingLayout& RuntimeShaderDataSource::binding_layout() const {
    return m_bindingLayout;
}

const VertexLayout& RuntimeShaderDataSource::vertex_layout() const {
    return m_vertexLayout;
}

uint32_t RuntimeShaderDataSource::binding_index(std::string_view name) const {
    for (uint32_t i = 0; i < static_cast<uint32_t>(m_bindingLayout.size()); i++) {
        if (m_bindingLayout[i].name == name) {
            return i;
        }
    }
    throw std::out_of_range(std::string("RuntimeShaderDataSource: no binding named '") + std::string(name) + "'");
}

duk::hash::Hash RuntimeShaderDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, m_moduleMask);
    for (auto& [type, code]: m_shaderModules) {
        duk::hash::hash_combine(hash, type);
        duk::hash::hash_combine(hash, code.data(), code.size());
    }
    return hash;
}

}// namespace duk::rhi
