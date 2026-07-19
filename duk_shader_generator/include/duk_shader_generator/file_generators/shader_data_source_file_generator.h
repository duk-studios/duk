/// shader_data_source_file_generator.h

#ifndef DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H

#include <duk_shader_generator/file_generators/file_generator.h>
#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/options.h>

#include <string>

namespace duk::shader_generator {

class ShaderDataSourceFileGenerator : public FileGenerator {
public:
    ShaderDataSourceFileGenerator(const Options& options, const rhi::RuntimeShaderDataSource& shaderDataSource);

private:
    // --- shader data source header ---
    void generate_header(std::ostringstream& oss) const;

    void generate_class_declaration(std::ostringstream& oss) const;

    // --- shader data source source ---
    void generate_source(std::ostringstream& oss) const;

    void generate_class_definition(std::ostringstream& oss) const;

    std::string header_include_path() const;

private:
    const Options& m_options;
    const rhi::RuntimeShaderDataSource& m_shaderDataSource;
    std::string m_fileName;
    std::string m_className;
};

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
