//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <duk_serial/json/serializer.h>

#include <duk_tools/file.h>

namespace duk::renderer {

ShaderPipelineHandler::ShaderPipelineHandler()
    : ResourceHandlerT("spp") {
}

bool ShaderPipelineHandler::accepts(const std::string& extension) const {
    return extension == ".spp";
}

duk::resource::Handle<ShaderPipeline> ShaderPipelineHandler::load(ShaderPipelinePool* pool, const resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("File '{}' does not exist", path.string()));
    }

    auto shaderPipelineDataJson = duk::tools::load_text(path);

    auto shaderPipelineData = duk::serial::read_json<ShaderPipelineData>(shaderPipelineDataJson);

    return pool->create(id, &shaderPipelineData);
}

}// namespace duk::renderer
