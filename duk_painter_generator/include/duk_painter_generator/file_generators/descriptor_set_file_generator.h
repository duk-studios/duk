/// 11/11/2023
/// descriptor_set_generator.h

#ifndef DUK_PAINTER_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H
#define DUK_PAINTER_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H

#include <duk_painter_generator/file_generators/file_generator.h>
#include <duk_painter_generator/file_generators/shader_data_source_file_generator.h>

namespace duk::painter_generator {

class DescriptorSetFileGenerator : public FileGenerator {
public:
    DescriptorSetFileGenerator(const Parser& parser, const Reflector& reflector, const ShaderDataSourceFileGenerator& shaderDataSourceFileGenerator);

private:

    void generate_header_file_content(std::ostringstream& oss);

    void generate_source_file_content(std::ostringstream& oss);

    void generate_class_declaration(std::ostringstream& oss, const SetReflection& descriptorSet);

    void generate_class_definition(std::ostringstream& oss);

private:
    const Parser& m_parser;
    const Reflector& m_reflector;
    const ShaderDataSourceFileGenerator& m_shaderDataSourceFileGenerator;
    std::string m_fileName;
    std::string m_className;
};

}

#endif // DUK_PAINTER_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H

