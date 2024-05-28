//
// Created by Ricardo on 09/05/2024.
//

#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <duk_tools/file.h>

namespace duk::renderer {

namespace detail {

static void load_shader_stage(rhi::StdShaderDataSource& shaderDataSource, duk::rhi::ShaderModule::Bits shaderModule, const std::filesystem::path& pipelinePath, const std::filesystem::path& relativeModulePath) {
    const auto modulePath = pipelinePath.parent_path() / relativeModulePath;
    const auto code = duk::tools::load_bytes(modulePath);
    shaderDataSource.insert_spir_v_code(shaderModule, code);
}

}

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

    duk::rhi::StdShaderDataSource shaderDataSource;
    detail::load_shader_stage(shaderDataSource, duk::rhi::ShaderModule::VERTEX, path, shaderPipelineData.shader.vertexPath);
    detail::load_shader_stage(shaderDataSource, duk::rhi::ShaderModule::FRAGMENT, path, shaderPipelineData.shader.fragmentPath);

    return pool->create(id, &shaderDataSource, shaderPipelineData.settings);
}

}
