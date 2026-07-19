/// generator.cpp

#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/options.h>
#include <duk_shader_generator/file_generators/shader_data_source_file_generator.h>

#include <duk_rhi/shader_compiler.h>

#include <filesystem>
#include <fstream>
#include <regex>
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

void generate(const Options& options) {
    std::unordered_map<rhi::ShaderModule::Bits, std::string> moduleSources;

    rhi::ShaderCompilerCreateInfo compilerInfo;
    compilerInfo.optimizationLevel = options.optimizationLevel;
    for (const auto& dir: options.includeDirectories) {
        compilerInfo.includeDirectories.emplace_back(dir);
    }

    for (const auto& [stage, path]: options.modulePaths) {
        moduleSources[stage] = detail::load_text_file(path);
    }

    rhi::ShaderCompiler compiler(compilerInfo);

    const auto shaderDataSource = rhi::compile(compiler, moduleSources);
    if (!shaderDataSource) {
        throw std::runtime_error("failed to compile shader source " + shaderDataSource.error().message);
    }
    ShaderDataSourceFileGenerator generator(options, shaderDataSource.value());
}

}// namespace duk::shader_generator
