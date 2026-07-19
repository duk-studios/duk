/// options.h

#ifndef DUK_SHADER_GENERATOR_OPTIONS_H
#define DUK_SHADER_GENERATOR_OPTIONS_H

#include <duk_rhi/shader.h>
#include <duk_rhi/shader_compiler.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace duk::shader_generator {

struct Options {
    // Output config
    std::string outputSourceDirectory;
    std::string outputIncludeDirectory;
    std::string outputHeaderIncludePrefix;
    std::string outputNamespace;
    std::string outputShaderName;

    // Input
    std::vector<std::string> includeDirectories;
    std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> modulePaths;

    duk::rhi::OptimizationLevel optimizationLevel = duk::rhi::OptimizationLevel::PERFORMANCE;
};

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_OPTIONS_H
