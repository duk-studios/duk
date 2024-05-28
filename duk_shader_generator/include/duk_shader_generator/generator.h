/// 14/10/2023
/// generator.h

#ifndef DUK_SHADER_GENERATOR_GENERATOR_H
#define DUK_SHADER_GENERATOR_GENERATOR_H

#include <duk_shader_generator/parser.h>
#include <duk_shader_generator/reflector.h>

namespace duk::material_generator {

class Generator {
public:
    Generator(const Parser& parser, const Reflector& reflector);

    ~Generator();
};

}// namespace duk::material_generator

#endif// DUK_SHADER_GENERATOR_GENERATOR_H
