/// 04/11/2023
/// global_types_file_generator.h

#ifndef DUK_PAINTER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H
#define DUK_PAINTER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H

#include <duk_painter_generator/file_generators/types_file_generator.h>

namespace duk::painter_generator {

class GlobalTypesFileGenerator: public TypesFileGenerator {
public:

    GlobalTypesFileGenerator(const Parser& parser, const Reflector& reflector);

private:

    std::vector<BindingReflection> extract_global_bindings();

    void generate_file_content(std::ostringstream& oss, const BindingReflection& binding);


};

}
#endif // DUK_PAINTER_GENERATOR_GLOBAL_TYPES_FILE_GENERATOR_H

