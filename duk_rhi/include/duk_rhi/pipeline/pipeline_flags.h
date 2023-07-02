/// 16/06/2023
/// pipeline_stages.h

#ifndef DUK_RHI_PIPELINE_STAGES_H
#define DUK_RHI_PIPELINE_STAGES_H

#include <cstdint>

namespace duk::rhi {

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

struct Access {
    enum Bits {
        NONE = 0,
        INDIRECT_COMMAND_READ = 0x00000001,
        INDEX_READ = 0x00000002,
        VERTEX_ATTRIBUTE_READ = 0x00000004,
        UNIFORM_READ = 0x00000008,
        INPUT_ATTACHMENT_READ = 0x00000010,
        SHADER_READ = 0x00000020,
        SHADER_WRITE = 0x00000040,
        COLOR_ATTACHMENT_READ = 0x00000080,
        COLOR_ATTACHMENT_WRITE = 0x00000100,
        DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
        DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
        TRANSFER_READ = 0x00000800,
        TRANSFER_WRITE = 0x00001000,
        HOST_READ = 0x00002000,
        HOST_WRITE = 0x00004000,
        MEMORY_READ = 0x00008000,
        MEMORY_WRITE = 0x00010000
    };
    static constexpr auto kCount = 18;
    using Mask = uint32_t;
};

}

#endif // DUK_RHI_PIPELINE_STAGES_H

