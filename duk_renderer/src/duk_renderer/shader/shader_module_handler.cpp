//
// Created by Ricardo on 19/06/2024.
//

#include <duk_renderer/shader/shader_module_handler.h>
#include <duk_tools/file.h>

namespace duk::renderer {

ShaderModuleHandler::ShaderModuleHandler()
    : ResourceHandlerT("spv") {
}

bool ShaderModuleHandler::accepts(const std::string& extension) const {
    return extension == ".spv";
}

duk::resource::Handle<ShaderModule> ShaderModuleHandler::load(ShaderModulePool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto code = duk::tools::load_bytes(path);
    ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.code = code.data();
    shaderModuleCreateInfo.size = code.size();
    return pool->insert(id, std::make_shared<ShaderModule>(shaderModuleCreateInfo));
}

}// namespace duk::renderer
