/// generator.cpp

#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/file_generators/shader_data_source_generator.h>
#include <duk_shader_generator/options.h>

#include <duk_rhi/shader_compiler.h>

#include <fstream>
#include <stdexcept>
namespace duk::shader_generator {

namespace detail {

static std::string load_text_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("failed to open shader source: " + path);
    }
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

}// namespace detail

duk::rhi::RuntimeShaderDataSource compile(const Options& options) {
    std::unordered_map<rhi::ShaderModule::Bits, std::string> sources;
    for (const auto& [stage, path]: options.inputGlslPaths) {
        sources[stage] = detail::load_text_file(path);
    }

    rhi::ShaderCompilerCreateInfo createInfo;
    createInfo.optimizationLevel = options.optimizationLevel;
    for (const auto& dir: options.includeDirectories) {
        createInfo.includeDirectories.emplace_back(dir);
    }

    return rhi::compile(createInfo, sources);
}

void generate(const Options& options) {
    const auto source = compile(options);
    write_shader_data_source(options, source);
}

}// namespace duk::shader_generator
