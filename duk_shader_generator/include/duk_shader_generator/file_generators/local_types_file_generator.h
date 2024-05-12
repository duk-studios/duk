/// 04/11/2023
/// local_types_file_generator.h

#ifndef DUK_SHADER_GENERATOR_LOCAL_TYPES_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_LOCAL_TYPES_FILE_GENERATOR_H

#include <duk_shader_generator/file_generators/types_file_generator.h>

namespace duk::material_generator {

class LocalTypesFileGenerator : public TypesFileGenerator {
public:
    LocalTypesFileGenerator(const Parser& parser, const Reflector& reflector);

private:
    void generate_file_content(std::ostringstream& oss);

    std::vector<TypeReflection> extract_local_types(std::vector<BindingReflection>& bindings);

    std::vector<BindingReflection> extract_local_bindings();
};

}// namespace duk::material_generator

#endif// DUK_SHADER_GENERATOR_LOCAL_TYPES_FILE_GENERATOR_H
