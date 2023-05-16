/// 18/04/2023
/// shader.h

#ifndef DUK_RENDERER_SHADER_H
#define DUK_RENDERER_SHADER_H

#include <cstdint>

namespace duk::renderer {

class Shader {
public:
    struct ModuleType {
        enum Bits : uint32_t {
            VERTEX = 1 << 0,
            TESSELATION_CONTROL = 1 << 1,
            TESSELATION_EVALUATION = 1 << 2,
            GEOMETRY = 1 << 3,
            FRAGMENT = 1 << 4,
            COMPUTE = 1 << 5,
        };
    };
    using ModuleMask = uint32_t;

};

}

#endif // DUK_RENDERER_SHADER_H
