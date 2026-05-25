/// shader_data_source_file_generator.h

#ifndef DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H

#include <duk_shader_generator/file_generators/file_generator.h>
#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/options.h>

namespace duk::shader_generator {

class ShaderDataSourceFileGenerator : public FileGenerator {
public:
    ShaderDataSourceFileGenerator(const Options& options, const GeneratedShaderData& data);

private:
    void generate_header_file(std::ostringstream& oss);

    void generate_source_file(std::ostringstream& oss);

    void generate_class_declaration(std::ostringstream& oss);

    void generate_class_definition(std::ostringstream& oss);

private:
    const Options& m_options;
    const GeneratedShaderData& m_data;
    std::string m_fileName;
    std::string m_className;
    std::string m_headerIncludePath;
};

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
