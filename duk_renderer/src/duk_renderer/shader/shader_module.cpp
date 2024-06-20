//
// Created by Ricardo on 19/06/2024.
//

#include <duk_renderer/shader/shader_module.h>

namespace duk::renderer {

ShaderModule::ShaderModule(const ShaderModuleCreateInfo& shaderModuleCreateInfo)
    : m_code(shaderModuleCreateInfo.code, shaderModuleCreateInfo.code + shaderModuleCreateInfo.size) {
}

const uint8_t* ShaderModule::data() const {
    return m_code.data();
}

size_t ShaderModule::size() const {
    return m_code.size();
}

}// namespace duk::renderer
