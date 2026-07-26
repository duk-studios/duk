//
// Created by rov on 31/05/2026.
//
#include <duk_renderer/draw_context.h>

#include <duk_math/frustum.h>

namespace duk::renderer {

void DrawBatch::submit(rhi::CommandContext& commandContext, const DrawState& state, const rhi::DrawParams& drawParams) {
    update_state(commandContext, state);
    if (!m_drawIndexedParams.empty()) {
        flush(commandContext);
    }
    m_drawParams.push_back(drawParams);
}

void DrawBatch::submit(rhi::CommandContext& commandContext, const DrawState& state, const rhi::DrawIndexedParams& drawParams) {
    update_state(commandContext, state);
    if (!m_drawParams.empty()) {
        flush(commandContext);
    }
    m_drawIndexedParams.push_back(drawParams);
}

void DrawBatch::flush(rhi::CommandContext& commandContext) {
    if (!m_drawParams.empty()) {
        commandContext.draw_indirect(m_drawParams);
        m_drawParams.clear();
    }
    if (!m_drawIndexedParams.empty()) {
        commandContext.draw_indexed_indirect(m_drawIndexedParams);
        m_drawIndexedParams.clear();
    }
}

void DrawBatch::update_state(rhi::CommandContext& commandContext, const DrawState& state) {
    if (m_state.shader != state.shader || m_state.pipeline != state.pipeline) {
        flush(commandContext);
        commandContext.bind_render_shader(state.shader, state.pipeline);
        m_state.shader = state.shader;
        m_state.pipeline = state.pipeline;
        m_state.resources = {};
        m_state.input = {};
    }
    if (m_state.resources != state.resources) {
        flush(commandContext);
        commandContext.bind_resources(state.resources);
        m_state.resources = state.resources;
    }
    if (m_state.input != state.input) {
        flush(commandContext);
        commandContext.bind_input(state.input);
        m_state.input = state.input;
    }
}


DrawGroup::DrawGroup(void* context, Submit* submit, Flush* flush)
    : m_context(context)
    , m_submit(submit)
    , m_flush(flush) {
}

void DrawGroup::submit(rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t groupEntryIndex) const {
    m_submit(m_context, commandContext, drawParams, groupEntryIndex);
}

void DrawGroup::flush(rhi::CommandContext& commandContext) const {
    m_flush(m_context, commandContext);
}

uint32_t DrawContext::add_group(const DrawGroup& group) {
    m_groups.push_back(group);
    return m_groups.size() - 1;
}

void DrawContext::add_entry(const DrawEntry& entry) {
    m_entries.push_back(entry);
}

void DrawContext::draw(rhi::CommandContext& commandContext, const DrawParams& drawParams) {

    std::vector<uint32_t> activeEntries;
    activeEntries.reserve(m_entries.size());

    // cull
    {
        auto frustum = math::Frustum(drawParams.view, drawParams.proj);
        for (uint32_t entryIndex = 0; entryIndex < m_entries.size(); entryIndex++) {
            const auto& entry = m_entries[entryIndex];
            if (frustum.intersects(entry.aabb)) {
                activeEntries.push_back(entryIndex);
            }
        }
    }

    // sort
    {
        std::ranges::sort(activeEntries, [this](auto lhs, auto rhs) -> bool {
            const auto& lhsEntry = m_entries[lhs];
            const auto& rhsEntry = m_entries[rhs];
            return lhsEntry.sortKey < rhsEntry.sortKey;
        });
    }

    // draw
    {
        const DrawGroup* currentGroup{nullptr};
        for (auto entryIndex : activeEntries) {
            const auto& entry = m_entries[entryIndex];
            if (entry.groupIndex >= m_groups.size()) {
                duk::log::warn("Invalid group index '{}' submitted for drawing");
                continue;
            }
            auto& group = m_groups[entry.groupIndex];
            if (currentGroup != &group) {
                if (currentGroup) {
                    currentGroup->flush(commandContext);
                }
                currentGroup = &group;
            }

            currentGroup->submit(commandContext, drawParams, entry.groupEntryIndex);
        }
        if (currentGroup) {
            currentGroup->flush(commandContext);
        }
    }
}

void DrawContext::clear() noexcept {
    m_groups.clear();
    m_entries.clear();
}

}
