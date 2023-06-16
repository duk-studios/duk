/// 18/04/2023
/// pipeline.h

#ifndef DUK_RENDERER_PIPELINE_H
#define DUK_RENDERER_PIPELINE_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <glm/vec2.hpp>

#include <cstdint>

namespace duk::renderer {

class Shader;

class GraphicsPipeline {
public:

    struct Viewport {
        glm::vec2 extent;
        glm::vec2 offset;
        float minDepth;
        float maxDepth;
    };

    struct Scissor {
        glm::u32vec2 extent;
        glm::ivec2 offset;
    };

    struct CullMode {
        enum Bits : uint32_t {
            NONE = 0,
            FRONT = 1 << 0,
            BACK = 1 << 1,
        };
        static constexpr uint32_t kCount = 3;
        using Mask = uint32_t;
    };

    struct Blend {
        enum class Factor {
            ZERO = 0,
            ONE = 1,
            SRC_COLOR = 2,
            ONE_MINUS_SRC_COLOR = 3,
            DST_COLOR = 4,
            ONE_MINUS_DST_COLOR = 5,
            SRC_ALPHA = 6,
            ONE_MINUS_SRC_ALPHA = 7,
            DST_ALPHA = 8,
            ONE_MINUS_DST_ALPHA = 9,
            CONSTANT_COLOR = 10,
            ONE_MINUS_CONSTANT_COLOR = 11,
            CONSTANT_ALPHA = 12,
            ONE_MINUS_CONSTANT_ALPHA = 13,
            SRC_ALPHA_SATURATE = 14,
            SRC1_COLOR = 15,
            ONE_MINUS_SRC1_COLOR = 16,
            SRC1_ALPHA = 17,
            ONE_MINUS_SRC1_ALPHA = 18,
        };

        enum class Operator {
            ADD = 0,
            SUBTRACT = 1,
            REVERSE_SUBTRACT = 2,
            MIN = 3,
            MAX = 4,
        };

        Factor srcColorBlendFactor;
        Factor dstColorBlendFactor;
        Operator colorBlendOp;
        Factor srcAlphaBlendFactor;
        Factor dstAlphaBlendFactor;
        Operator alphaBlendOp;
        bool enabled;
    };

    enum class Topology {
        TRIANGLE_LIST = 0,
        TRIANGLE_STRIP = 1,
        TRIANGLE_FAN = 2,
        TRIANGLE_LIST_WITH_ADJACENCY = 3,
        TRIANGLE_STRIP_WITH_ADJACENCY = 4,
        POINT_LIST = 5,
        LINE_LIST = 6,
        LINE_STRIP = 7,
        LINE_LIST_WITH_ADJACENCY = 8,
        LINE_STRIP_WITH_ADJACENCY = 9,
        PATCH_LIST = 10,
    };

    enum class FillMode {
        FILL = 0,
        LINE = 1,
        POINT = 2
    };

public:

    virtual ~GraphicsPipeline();

    virtual void set_viewport(const Viewport& viewport) = 0;

    DUK_NO_DISCARD virtual const Viewport& viewport() const = 0;

    void set_viewport(const glm::vec2& extent, const glm::vec2& offset = {}, float minDepth = 0.0f, float maxDepth = 1.0f);

    virtual void set_scissor(const Scissor& scissor) = 0;

    DUK_NO_DISCARD virtual const Scissor& scissor() const = 0;

    void set_scissor(const glm::ivec2& extent, const glm::u32vec2& offset = {});

    virtual void set_blend(const Blend& blend) = 0;

    DUK_NO_DISCARD virtual const Blend& blend() const = 0;

    void set_blend_src_color_factor(Blend::Factor srcColorBlendFactor);

    void set_blend_dst_color_factor(Blend::Factor dstColorBlendFactor);

    void set_blend_color_op(Blend::Operator colorBlendOp);

    void set_blend_src_alpha_factor(Blend::Factor srcAlphaBlendFactor);

    void set_blend_dst_alpha_factor(Blend::Factor dstAlphaBlendFactor);

    void set_blend_alpha_op(Blend::Operator alphaBlendOp);

    void set_blend_enabled(bool enabled);

    virtual void set_cull_mode(CullMode::Mask cullModeMask) = 0;

    virtual CullMode::Mask cull_mode() = 0;

    virtual void set_topology(Topology topology) = 0;

    DUK_NO_DISCARD virtual Topology topology() const = 0;

    virtual void set_fill_mode(FillMode fillMode) = 0;

    DUK_NO_DISCARD virtual FillMode fill_mode() const = 0;

    virtual void flush() = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;

};

}

#endif // DUK_RENDERER_PIPELINE_H

