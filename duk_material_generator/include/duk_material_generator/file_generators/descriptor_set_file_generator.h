/// 11/11/2023
/// descriptor_set_generator.h

#ifndef DUK_MATERIAL_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H
#define DUK_MATERIAL_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H

#include <duk_material_generator/file_generators/types_file_generator.h>
#include <duk_material_generator/file_generators/shader_data_source_file_generator.h>

namespace duk::material_generator {

class DescriptorSetFileGenerator : public TypesFileGenerator {
public:
    DescriptorSetFileGenerator(const Parser& parser, const Reflector& reflector, const ShaderDataSourceFileGenerator& shaderDataSourceFileGenerator);

private:

    void generate_header_file_content(std::ostringstream& oss);

    void generate_source_file_content(std::ostringstream& oss);

    void generate_class_declaration(std::ostringstream& oss, const SetReflection& descriptorSet);

    void generate_class_definition(std::ostringstream& oss);

private:
    const ShaderDataSourceFileGenerator& m_shaderDataSourceFileGenerator;
    std::string m_fileName;
    std::string m_className;
};

}

#endif // DUK_MATERIAL_GENERATOR_DESCRIPTOR_SET_FILE_GENERATOR_H

