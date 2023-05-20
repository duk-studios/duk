/// 18/04/2023
/// shader.h

#ifndef DUK_RENDERER_SHADER_H
#define DUK_RENDERER_SHADER_H

#include <duk_macros/macros.h>
#include <duk_hash/hash.h>

#include <cstdint>

namespace duk::renderer {

class Shader {
public:
    struct ModuleType {
        enum Bits : uint32_t {
            VERTEX = 1 << 0,
            TESSELLATION_CONTROL = 1 << 1,
            TESSELLATION_EVALUATION = 1 << 2,
            GEOMETRY = 1 << 3,
            FRAGMENT = 1 << 4,
            COMPUTE = 1 << 5,
        };
        static constexpr uint32_t kCount = 6;
    };
    using ModuleMask = uint32_t;

    virtual ~Shader();

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;

};

}

#endif // DUK_RENDERER_SHADER_H
