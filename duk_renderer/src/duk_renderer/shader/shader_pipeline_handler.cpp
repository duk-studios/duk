//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

ShaderPipelineHandler::ShaderPipelineHandler()
    : TextHandlerT("spp") {
}

bool ShaderPipelineHandler::accepts(const std::string& extension) const {
    return extension == ".spp";
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineHandler::load_from_text(ShaderPipelinePool* pool, const resource::Id& id, const std::string_view& text) {
    auto shaderPipelineData = duk::serial::read_json<ShaderPipelineData>(text);
    return pool->create(id, &shaderPipelineData);
}

}// namespace duk::renderer
