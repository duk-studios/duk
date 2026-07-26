//
// Created by rov on 31/05/2026.
//

#ifndef DUK_RENDERER_DRAW_CONTEXT_H
#define DUK_RENDERER_DRAW_CONTEXT_H

#include <duk_rhi/command_context.h>
#include <duk_objects/id.h>
#include <duk_math/aabb.h>

namespace duk::renderer {

struct DrawState {
    const rhi::Shader* shader{};
    rhi::PipelineState pipeline{};
    rhi::ShaderBindings resources{};
    rhi::ShaderInput input{};
};

class DrawBatch {
public:
    void submit(rhi::CommandContext& commandContext, const DrawState& state, const rhi::DrawParams& drawParams);

    void submit(rhi::CommandContext& commandContext, const DrawState& state, const rhi::DrawIndexedParams& drawParams);

    void flush(rhi::CommandContext& commandContext);
private:

    void update_state(rhi::CommandContext& commandContext, const DrawState& state);

private:
    DrawState m_state;
    std::vector<rhi::DrawParams> m_drawParams;
    std::vector<rhi::DrawIndexedParams> m_drawIndexedParams;
};

struct DrawParams {
    glm::mat4 proj;
    glm::mat4 view;
};

class DrawGroup {
public:
    using Submit = void(void* context, rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t groupEntryIndex);
    using Flush = void(void* context, rhi::CommandContext& commandContext);

    DrawGroup(void* context, Submit* submit, Flush* flush);

    void submit(rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t groupEntryIndex) const;

    void flush(rhi::CommandContext& commandContext) const;

private:
    void* m_context;
    Submit* m_submit;
    Flush* m_flush;
};

struct DrawEntry {
    uint32_t groupIndex;
    uint32_t groupEntryIndex;
    uint64_t sortKey;
    math::AABB aabb;
};

class DrawContext {
public:

    // returns group index
    DUK_NO_DISCARD uint32_t add_group(const DrawGroup& group);

    void add_entry(const DrawEntry& entry);

    void draw(rhi::CommandContext& commandContext, const DrawParams& drawParams);

    void clear() noexcept;

private:
    std::vector<DrawGroup> m_groups;
    std::vector<DrawEntry> m_entries;
};

}

#endif //DUK_RENDERER_DRAW_CONTEXT_H
