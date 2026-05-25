/// options.h

#ifndef DUK_SHADER_GENERATOR_OPTIONS_H
#define DUK_SHADER_GENERATOR_OPTIONS_H

#include <duk_rhi/shader.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace duk::shader_generator {

struct Options {
    std::string outputSourceDirectory;
    std::string outputIncludeDirectory;
    std::string shaderName;
    std::string outputNamespace;
    std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> inputGlslPaths;
    std::vector<std::string> includeDirectories;
    bool printDebugInfo = false;
};

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_OPTIONS_H

