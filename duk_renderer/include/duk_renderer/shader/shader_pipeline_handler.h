//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_HANDLER_H
#define DUK_RENDERER_SHADER_PIPELINE_HANDLER_H

#include <duk_renderer/shader/shader_pipeline_pool.h>

#include <duk_resource/handler.h>

namespace duk::renderer {

class ShaderPipelineHandler : public duk::resource::ResourceHandlerT<ShaderPipelinePool> {
public:

    ShaderPipelineHandler();

    bool accepts(const std::string& extension) const override;

protected:
    void load(ShaderPipelinePool* pool, const resource::Id& id, const std::filesystem::path& path) override;

private:
};

}

#endif //DUK_RENDERER_SHADER_PIPELINE_HANDLER_H
