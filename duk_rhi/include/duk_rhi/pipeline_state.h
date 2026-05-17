/// pipeline_state.h

#ifndef DUK_RHI_PIPELINE_STATE_H
#define DUK_RHI_PIPELINE_STATE_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <cstdint>

namespace duk::rhi {

enum class StoreOp {
    STORE = 0,
    DONT_CARE
};

enum class LoadOp {
    LOAD = 0,
    CLEAR,
    DONT_CARE
};

struct PipelineState {

    struct Viewport {
        glm::vec2 extent{0.0f, 0.0f};
        glm::vec2 offset{0.0f, 0.0f};
        float minDepth{0.0f};
        float maxDepth{1.0f};
    };

    struct Scissor {
        glm::u32vec2 extent{0, 0};
        glm::ivec2 offset{0, 0};
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

        Factor srcColorBlendFactor{Factor::ONE};
        Factor dstColorBlendFactor{Factor::ZERO};
        Operator colorBlendOp{Operator::ADD};
        Factor srcAlphaBlendFactor{Factor::ONE};
        Factor dstAlphaBlendFactor{Factor::ZERO};
        Operator alphaBlendOp{Operator::ADD};
        bool enabled{false};
    };

    struct Rasterizer {
        struct CullMode {
            enum Bits : uint32_t {
                NONE = 0,
                FRONT = 1 << 0,
                BACK = 1 << 1,
            };

            static constexpr uint32_t kCount = 3;
            using Mask = uint32_t;
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
            POINT = 2,
        };

        enum class Origin {
            DEFAULT     = 0, // no transformation; renders in the native coordinate origin of the underlying API
            UPPER_LEFT  = 1, // force Y-down, origin at top-left  (native Vulkan / D3D)
            BOTTOM_LEFT = 2, // force Y-up,   origin at bottom-left (OpenGL convention)
        };

        CullMode::Mask cullMode{CullMode::NONE};
        Topology topology{Topology::TRIANGLE_LIST};
        FillMode fillMode{FillMode::FILL};
        Origin origin{Origin::DEFAULT};
        bool depthTesting{false};
    };

    Viewport viewport{};
    Scissor scissor{};
    Blend blend{};
    Rasterizer rasterizer{};
};

}// namespace duk::rhi

template<>
struct std::hash<duk::rhi::PipelineState::Viewport> {
    size_t operator()(const duk::rhi::PipelineState::Viewport& viewport) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, viewport.extent);
        duk::hash::hash_combine(hash, viewport.offset);
        duk::hash::hash_combine(hash, viewport.minDepth);
        duk::hash::hash_combine(hash, viewport.maxDepth);
        return hash;
    }
};

template<>
struct std::hash<duk::rhi::PipelineState::Scissor> {
    size_t operator()(const duk::rhi::PipelineState::Scissor& scissor) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, scissor.extent);
        duk::hash::hash_combine(hash, scissor.offset);
        return hash;
    }
};

template<>
struct std::hash<duk::rhi::PipelineState::Blend> {
    size_t operator()(const duk::rhi::PipelineState::Blend& blend) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.srcColorBlendFactor));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.dstColorBlendFactor));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.colorBlendOp));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.srcAlphaBlendFactor));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.dstAlphaBlendFactor));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(blend.alphaBlendOp));
        duk::hash::hash_combine(hash, blend.enabled);
        return hash;
    }
};

template<>
struct std::hash<duk::rhi::PipelineState::Rasterizer> {
    size_t operator()(const duk::rhi::PipelineState::Rasterizer& rasterizer) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, rasterizer.cullMode);
        duk::hash::hash_combine(hash, static_cast<uint32_t>(rasterizer.topology));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(rasterizer.fillMode));
        duk::hash::hash_combine(hash, static_cast<uint32_t>(rasterizer.origin));
        duk::hash::hash_combine(hash, rasterizer.depthTesting);
        return hash;
    }
};

template<>
struct std::hash<duk::rhi::PipelineState> {
    size_t operator()(const duk::rhi::PipelineState& state) const noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, state.viewport);
        duk::hash::hash_combine(hash, state.scissor);
        duk::hash::hash_combine(hash, state.blend);
        duk::hash::hash_combine(hash, state.rasterizer);
        return hash;
    }
};

#endif// DUK_RHI_PIPELINE_STATE_H

