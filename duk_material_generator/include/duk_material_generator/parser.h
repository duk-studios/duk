/// 14/10/2023
/// parser.h

#ifndef DUK_MATERIAL_GENERATOR_PARSER_H
#define DUK_MATERIAL_GENERATOR_PARSER_H

#include <duk_rhi/pipeline/shader.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace duk::material_generator {

class Parser {
public:
    using ShaderPaths = std::unordered_map<duk::rhi::Shader::Module::Bits, std::string>;
public:

    Parser(int argc, char* argv[]);

    ~Parser();

    DUK_NO_DISCARD const std::string& output_source_directory() const;

    DUK_NO_DISCARD const std::string& output_globals_source_directory() const;

    DUK_NO_DISCARD const std::string& output_include_directory() const;

    DUK_NO_DISCARD const std::string& output_globals_include_directory() const;

    DUK_NO_DISCARD const std::string& output_material_name() const;

    DUK_NO_DISCARD const ShaderPaths& input_spv_paths() const;

    DUK_NO_DISCARD bool is_global_binding(const std::string& typeName) const;

    DUK_NO_DISCARD bool print_debug_info() const;

private:
    std::string m_outputSourceDirectory;
    std::string m_outputGlobalsSourceDirectory;
    std::string m_outputIncludeDirectory;
    std::string m_outputGlobalsIncludeDirectory;
    std::string m_outputMaterialName;
    ShaderPaths m_inputSpvPaths;
    std::unordered_set<std::string> m_globalBindings;
    bool m_printDebugInfo;
};

}

#endif // DUK_MATERIAL_GENERATOR_PARSER_H

