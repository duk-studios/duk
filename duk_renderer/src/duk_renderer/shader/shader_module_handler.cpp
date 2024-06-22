//
// Created by Ricardo on 19/06/2024.
//

#include <duk_renderer/shader/shader_module_handler.h>

namespace duk::renderer {

ShaderModuleHandler::ShaderModuleHandler()
    : ResourceHandlerT("spv") {
}

bool ShaderModuleHandler::accepts(const std::string& extension) const {
    return extension == ".spv";
}

duk::resource::Handle<ShaderModule> ShaderModuleHandler::load_from_memory(ShaderModulePool* pool, const resource::Id& id, const void* data, size_t size) {
    ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.code = static_cast<const uint8_t*>(data);
    shaderModuleCreateInfo.size = size;
    return pool->insert(id, std::make_shared<ShaderModule>(shaderModuleCreateInfo));
}

}// namespace duk::renderer
