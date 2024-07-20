//
// Created by Ricardo on 19/06/2024.
//

#include <duk_renderer/shader/shader_module_handler.h>

namespace duk::renderer {

ShaderModuleHandler::ShaderModuleHandler()
    : HandlerT("spv") {
}

bool ShaderModuleHandler::accepts(const std::string& extension) const {
    return extension == ".spv";
}

std::shared_ptr<ShaderModule> ShaderModuleHandler::load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) {
    ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.code = static_cast<const uint8_t*>(data);
    shaderModuleCreateInfo.size = size;
    return std::make_shared<ShaderModule>(shaderModuleCreateInfo);
}

}// namespace duk::renderer
