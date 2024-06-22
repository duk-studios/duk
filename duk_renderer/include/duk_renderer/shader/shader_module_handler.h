//
// Created by Ricardo on 19/06/2024.
//

#ifndef DUK_RENDERER_SHADER_MODULE_HANDLER_H
#define DUK_RENDERER_SHADER_MODULE_HANDLER_H

#include <duk_renderer/shader/shader_module_pool.h>
#include <duk_resource/handler.h>

namespace duk::renderer {

class ShaderModuleHandler : public resource::ResourceHandlerT<ShaderModulePool> {
public:
    ShaderModuleHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<ShaderModule> load_from_memory(ShaderModulePool* pool, const resource::Id& id, const void* data, size_t size) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_MODULE_HANDLER_H
