/// 14/10/2023
/// generator.cpp

#include <duk_painter_generator/generator.h>
#include <duk_painter_generator/file_generators/local_types_file_generator.h>
#include <duk_painter_generator/file_generators/global_types_file_generator.h>

namespace duk::painter_generator {

Generator::Generator(const Parser& parser, const Reflector& reflector) {
    LocalTypesFileGenerator localTypesFileGenerator(parser, reflector);
    GlobalTypesFileGenerator globalTypesFileGenerator(parser, reflector);
}

Generator::~Generator() = default;

}