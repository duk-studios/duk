/// 04/11/2023
/// shader_data_source_generator.h

#ifndef DUK_PAINTER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H
#define DUK_PAINTER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H

#include <duk_painter_generator/file_generators/file_generator.h>

namespace duk::painter_generator {

class ShaderDataSourceFileGenerator : public FileGenerator {
public:
    ShaderDataSourceFileGenerator(const Parser& parser, const Reflector& reflector);

private:

    void generate_header_file_content(std::ostringstream& oss);

    void generate_source_file_content(std::ostringstream& oss);

    void generate_class_declaration(std::ostringstream& oss);

    void generate_class_definition(std::ostringstream& oss);

private:
    const Parser& m_parser;
    const Reflector& m_reflector;
    std::string m_fileName;
    std::string m_className;
};

}

#endif // DUK_PAINTER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H

