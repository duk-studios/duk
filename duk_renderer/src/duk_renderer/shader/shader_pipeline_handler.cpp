//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/shader/shader_pipeline_data.h>
#include <duk_renderer/renderer.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

ShaderPipelineHandler::ShaderPipelineHandler()
    : TextHandlerT("spp") {
}

bool ShaderPipelineHandler::accepts(const std::string& extension) const {
    return extension == ".spp";
}

std::shared_ptr<ShaderPipeline> ShaderPipelineHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    const auto shaderPipelineData = duk::serial::read_json<ShaderPipelineData>(text);

    const auto renderer = globals->get<Renderer>();

    ShaderPipelineCreateInfo shaderPipelineCreateInfo = {};
    shaderPipelineCreateInfo.rhi = renderer->rhi();
    shaderPipelineCreateInfo.shaderPipelineData = &shaderPipelineData;

    return std::make_shared<ShaderPipeline>(shaderPipelineCreateInfo);
}

}// namespace duk::renderer
