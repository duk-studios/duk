/// 14/10/2023
/// generator.cpp

#include <duk_shader_generator/file_generators/shader_data_source_file_generator.h>
#include <duk_shader_generator/generator.h>

namespace duk::material_generator {

Generator::Generator(const Parser& parser, const Reflector& reflector) {
    ShaderDataSourceFileGenerator shaderDataSourceFileGenerator(parser, reflector);
}

Generator::~Generator() = default;

}// namespace duk::material_generator