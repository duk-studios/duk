/// 16/05/2023
/// std_shader_data_source.cpp

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <duk_macros/assert.h>

namespace duk::rhi {

StdShaderDataSource::StdShaderDataSource()
    : m_moduleMask(0) {
}

StdShaderDataSource::~StdShaderDataSource() = default;

ShaderModule::Mask StdShaderDataSource::module_mask() const {
    return m_moduleMask;
}

const std::vector<uint8_t>& StdShaderDataSource::shader_module_spir_v_code(ShaderModule::Bits type) const {
    DUK_ASSERT(m_moduleMask & type && "ShaderModule was not set");
    const auto& module = m_shaderModules.at(type);
    return module.spirVCode;
}

duk::hash::Hash StdShaderDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;

    duk::hash::hash_combine(hash, m_moduleMask);
    for (auto& [type, module]: m_shaderModules) {
        duk::hash::hash_combine(hash, type);
        duk::hash::hash_combine(hash, module.spirVCode.data(), module.spirVCode.size());
    }

    return hash;
}

void StdShaderDataSource::insert_spir_v_code(ShaderModule::Bits type, const uint8_t* data, size_t size) {
    DUK_ASSERT(!(m_moduleMask & type) && "ShaderModule was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode.insert(module.spirVCode.end(), data, data + size);
}

void StdShaderDataSource::insert_spir_v_code(ShaderModule::Bits type, const std::vector<uint8_t>& data) {
    DUK_ASSERT(!(m_moduleMask & type) && "ShaderModule was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode = data;
}

void StdShaderDataSource::insert_spir_v_code(ShaderModule::Bits type, std::vector<uint8_t>&& data) {
    DUK_ASSERT(!(m_moduleMask & type) && "ShaderModule was already set");
    m_moduleMask |= type;
    auto& module = m_shaderModules[type];
    module.spirVCode = std::move(data);
}

}// namespace duk::rhi