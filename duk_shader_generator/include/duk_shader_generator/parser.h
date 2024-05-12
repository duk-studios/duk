/// 14/10/2023
/// parser.h

#ifndef DUK_SHADER_GENERATOR_PARSER_H
#define DUK_SHADER_GENERATOR_PARSER_H

#include <duk_rhi/pipeline/shader.h>

#include <string>
#include <unordered_map>

namespace duk::material_generator {

class Parser {
public:
    using ShaderPaths = std::unordered_map<duk::rhi::ShaderModule::Bits, std::string>;

public:
    Parser(int argc, char* argv[]);

    ~Parser();

    DUK_NO_DISCARD const std::string& output_source_directory() const;

    DUK_NO_DISCARD const std::string& output_include_directory() const;

    DUK_NO_DISCARD const std::string& output_material_name() const;

    DUK_NO_DISCARD const ShaderPaths& input_spv_paths() const;

    DUK_NO_DISCARD bool print_debug_info() const;

private:
    std::string m_outputSourceDirectory;
    std::string m_outputIncludeDirectory;
    std::string m_outputMaterialName;
    ShaderPaths m_inputSpvPaths;
    bool m_printDebugInfo;
};

}// namespace duk::material_generator

#endif// DUK_SHADER_GENERATOR_PARSER_H
