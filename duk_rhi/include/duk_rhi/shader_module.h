//
// Created by rov on 09/08/2026.
//

#ifndef DUK_RHI_SHADER_MODULE_H
#define DUK_RHI_SHADER_MODULE_H

#include <cstdint>

namespace duk::rhi {

struct ShaderModule {
    enum Bits : uint32_t {
        VERTEX = 1 << 0,
        TESSELLATION_CONTROL = 1 << 1,
        TESSELLATION_EVALUATION = 1 << 2,
        GEOMETRY = 1 << 3,
        FRAGMENT = 1 << 4,
        COMPUTE = 1 << 5,
    };

    static constexpr uint32_t kCount = 6;
    using Mask = uint32_t;
};

}

#endif //DUK_RHI_SHADER_MODULE_H
