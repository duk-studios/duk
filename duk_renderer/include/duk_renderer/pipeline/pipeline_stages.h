/// 16/06/2023
/// pipeline_stages.h

#ifndef DUK_RENDERER_PIPELINE_STAGES_H
#define DUK_RENDERER_PIPELINE_STAGES_H

#include <cstdint>

namespace duk::renderer {

struct PipelineStage {

    enum Bits : uint32_t {
        NONE = 0,
        TOP_OF_PIPE = 1 << 0,
        DRAW_INDIRECT = 1 << 1,
        VERTEX_INPUT = 1 << 2,
        VERTEX_SHADER = 1 << 3,
        TESSELLATION_CONTROL_SHADER = 1 << 4,
        TESSELLATION_EVALUATION_SHADER = 1 << 5,
        GEOMETRY_SHADER = 1 << 6,
        FRAGMENT_SHADER = 1 << 7,
        EARLY_FRAGMENT_TESTS = 1 << 8,
        LATE_FRAGMENT_TESTS = 1 << 9,
        COLOR_ATTACHMENT_OUTPUT = 1 << 10,
        COMPUTE_SHADER = 1 << 11,
        TRANSFER = 1 << 12,
        BOTTOM_OF_PIPE = 1 << 13,
        HOST = 1 << 14,
        ALL_GRAPHICS = 1 << 15,
        ALL_COMMANDS = 1 << 16
    };
    static constexpr auto kCount = 18;
    using Mask = uint32_t;
};
}

#endif // DUK_RENDERER_PIPELINE_STAGES_H

