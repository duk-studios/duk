/// 14/10/2023
/// generator.cpp

#include <duk_material_generator/generator.h>
#include <duk_material_generator/file_generators/local_types_file_generator.h>
#include <duk_material_generator/file_generators/global_types_file_generator.h>
#include <duk_material_generator/file_generators/shader_data_source_file_generator.h>
#include <duk_material_generator/file_generators/descriptor_set_file_generator.h>

namespace duk::material_generator {

Generator::Generator(const Parser& parser, const Reflector& reflector) {
    GlobalTypesFileGenerator globalTypesFileGenerator(parser, reflector);
    ShaderDataSourceFileGenerator shaderDataSourceFileGenerator(parser, reflector);
    DescriptorSetFileGenerator descriptorSetFileGenerator(parser, reflector, shaderDataSourceFileGenerator);
}

Generator::~Generator() = default;

}