/// 14/10/2023
/// parser.h

#ifndef DUK_PAINTER_GENERATOR_PARSER_H
#define DUK_PAINTER_GENERATOR_PARSER_H

#include <duk_rhi/pipeline/shader.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace duk::painter_generator {

class Parser {
public:
    using ShaderPaths = std::unordered_map<duk::rhi::Shader::Module::Bits, std::string>;
public:

    Parser(int argc, char* argv[]);

    ~Parser();

    DUK_NO_DISCARD const std::string& output_src_directory() const;

    DUK_NO_DISCARD const std::string& output_include_directory() const;

    DUK_NO_DISCARD const std::string& output_painter_name() const;

    DUK_NO_DISCARD const ShaderPaths& input_spv_paths() const;

private:
    std::string m_outputSrcDirectory;
    std::string m_outputIncludeDirectory;
    std::string m_outputPainterName;
    ShaderPaths m_inputSpvPaths;
};

}

#endif // DUK_PAINTER_GENERATOR_PARSER_H

