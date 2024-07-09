//
// Created by Ricardo on 19/06/2024.
//

#ifndef DUK_RENDERER_SHADER_MODULE_HANDLER_H
#define DUK_RENDERER_SHADER_MODULE_HANDLER_H

#include <duk_renderer/shader/shader_module.h>
#include <duk_resource/handler.h>

namespace duk::renderer {

class ShaderModuleHandler : public resource::HandlerT<ShaderModule> {
public:
    ShaderModuleHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<ShaderModule> load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_MODULE_HANDLER_H
