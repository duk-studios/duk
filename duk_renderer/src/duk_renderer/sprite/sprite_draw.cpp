//
// Created by rov on 31/05/2026.
//

#include <duk_renderer/sprite/sprite_draw.h>

#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/material_slot.h>

namespace duk::renderer {

namespace detail {

static void context_submit(void* context, rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t groupEntryIndex) {
    const auto spriteContext = static_cast<SpriteContext*>(context);
    spriteContext->submit(commandContext, drawParams, groupEntryIndex);
}

static void context_flush(void* context, rhi::CommandContext& commandContext) {
    const auto spriteContext = static_cast<SpriteContext*>(context);
    spriteContext->flush(commandContext);
}

static DrawGroup make_sprite_group(SpriteContext& spriteContext) {
    return DrawGroup(&spriteContext, context_submit, context_flush);
}

static std::optional<SpriteEntry> make_sprite_entry(const SpriteRenderer& spriteRenderer) {
    auto& sprite = spriteRenderer.sprite;
    if (!sprite.valid()) {
        return std::nullopt;
    }

    const auto image = sprite->image();
    if (!image) {
        return std::nullopt;
    }

    SpriteEntry spriteEntry;
    spriteEntry.image = image.get();
    spriteEntry.spriteMetrics = sprite->compute_metrics(spriteRenderer.index);
    return spriteEntry;
}

static uint64_t compute_sort_key(const SpriteEntry& spriteEntry) {
    uint64_t key;
    return key;
}

static math::AABB compute_aabb(const SpriteMetrics& spriteMetrics) {
    math::AABB aabb;
    aabb.extend(glm::vec3(spriteMetrics.position.min, 0.0f));
    aabb.extend(glm::vec3(spriteMetrics.position.max, 0.0f));
    return aabb;
}

}

SpriteContext::SpriteContext(std::shared_ptr<SpriteCache> spriteCache)
    : m_spriteCache(std::move(spriteCache)) {

}

uint32_t SpriteContext::add_entry(const SpriteEntry& entry) {
    m_entries.push_back(entry);
    return m_entries.size() - 1;
}

void SpriteContext::submit(rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t entryIndex) {
    auto& entry = m_entries[entryIndex];
    auto mesh = m_spriteCache->mesh_for(commandContext, entry.spriteMetrics);
    if (!mesh) {
        duk::log::warn("Failed to create sprite mesh during draw submission");
        return;
    }

    DrawState drawState;
    drawState.input = make_shader_input(mesh->buffer(), 0);

    // material or context default
    drawState.shader = nullptr;

    // material + context + external
    // uploads per instance data
    drawState.pipeline = {};
    drawState.resources = {};

    rhi::DrawIndexedParams params;
    params.firstIndex = mesh->index_offset();
    params.indexCount = mesh->index_count();
    params.vertexOffset = mesh->vertex_offset();
    params.firstInstance = 0; // todo: use actual instance offset
    params.instanceCount = 1; // todo: support instancing for repeated sprites

    // bind input
    m_batch.submit(commandContext, drawState, params);
}

void SpriteContext::flush(rhi::CommandContext& commandContext) {
    // flush pending batch
    m_batch.flush(commandContext);
}

void add_sprite_draw_entries(SpriteContext& spriteContext, DrawContext& drawContext, objects::Objects& objects) {

    const auto spriteGroupIndex = drawContext.add_group(detail::make_sprite_group(spriteContext));

    for (auto [spriteRenderer] : objects.all_of<SpriteRenderer>()) {
        if (const auto spriteEntryOpt = detail::make_sprite_entry(*spriteRenderer)) {
            const auto& spriteEntry = spriteEntryOpt.value();

            DrawEntry drawEntry;
            drawEntry.groupIndex = spriteGroupIndex;
            drawEntry.groupEntryIndex = spriteContext.add_entry(spriteEntry);
            drawEntry.sortKey = detail::compute_sort_key(spriteEntry);
            drawEntry.aabb = detail::compute_aabb(spriteEntry.spriteMetrics);

            drawContext.add_entry(drawEntry);
        }
    }
}

}
