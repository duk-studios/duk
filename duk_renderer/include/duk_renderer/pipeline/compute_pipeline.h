/// 13/06/2023
/// compute_pipeline.h

#ifndef DUK_RENDERER_COMPUTE_PIPELINE_H
#define DUK_RENDERER_COMPUTE_PIPELINE_H

#include <cstdint>

namespace duk::renderer {

class ComputePipeline {
public:

    struct Stage {
        enum Bits : uint32_t {
            NONE = 0,
            TOP_OF_PIPE = 1 << 0,
            COMPUTE_SHADER = 1 << 1,
            BOTTOM_OF_PIPE = 1 << 2,
        };

        using Mask = uint32_t;
    };

    virtual ~ComputePipeline();


};

}

#endif // DUK_RENDERER_COMPUTE_PIPELINE_H

