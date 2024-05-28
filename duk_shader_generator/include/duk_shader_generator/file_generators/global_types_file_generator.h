/// 04/11/2023
/// global_types_file_generator.h

#ifndef DUK_SHADER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H

#include <duk_shader_generator/file_generators/types_file_generator.h>

namespace duk::material_generator {

class GlobalTypesFileGenerator : public TypesFileGenerator {
public:
    GlobalTypesFileGenerator(const Parser& parser, const Reflector& reflector);

private:
    std::vector<BindingReflection> extract_global_bindings();

    void generate_file_content(std::ostringstream& oss, const BindingReflection& binding);
};

}// namespace duk::material_generator
#endif// DUK_SHADER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H
