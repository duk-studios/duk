/// 14/10/2023
/// generator.h

#ifndef DUK_PAINTER_GENERATOR_GENERATOR_H
#define DUK_PAINTER_GENERATOR_GENERATOR_H

#include <duk_painter_generator/parser.h>
#include <duk_painter_generator/reflector.h>

namespace duk::painter_generator {

class Generator {
public:
    Generator(const Parser& parser, const Reflector& reflector);

    ~Generator();
};

}

#endif // DUK_PAINTER_GENERATOR_GENERATOR_H

