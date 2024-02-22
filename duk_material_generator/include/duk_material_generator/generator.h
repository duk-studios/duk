/// 14/10/2023
/// generator.h

#ifndef DUK_MATERIAL_GENERATOR_GENERATOR_H
#define DUK_MATERIAL_GENERATOR_GENERATOR_H

#include <duk_material_generator/parser.h>
#include <duk_material_generator/reflector.h>

namespace duk::material_generator {

class Generator {
public:
    Generator(const Parser& parser, const Reflector& reflector);

    ~Generator();
};

}// namespace duk::material_generator

#endif// DUK_MATERIAL_GENERATOR_GENERATOR_H
