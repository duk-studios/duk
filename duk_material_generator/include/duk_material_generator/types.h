/// 15/10/2023
/// types.h

#ifndef DUK_MATERIAL_GENERATOR_TYPES_H
#define DUK_MATERIAL_GENERATOR_TYPES_H

#include <string>

namespace duk::material_generator {

const std::string& glsl_to_cpp(const std::string& glslTypeName);

bool is_builtin_glsl_type(const std::string& glslTypeName);

}

#endif // DUK_MATERIAL_GENERATOR_TYPES_H

