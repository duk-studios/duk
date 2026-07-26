//
// Created by rov on 31/05/2026.
//

#ifndef DUK_RENDERER_SPRITE_DRAW_H
#define DUK_RENDERER_SPRITE_DRAW_H

#include <duk_renderer/draw_context.h>
#include <duk_renderer/sprite/sprite_cache.h>

#include <duk_objects/objects.h>

namespace duk::renderer {

struct SpriteRenderer;

struct SpriteEntry {
    SpriteMetrics spriteMetrics;
    const Image* image;
};

class SpriteContext {
public:
    explicit SpriteContext(std::shared_ptr<SpriteCache> spriteCache);

    uint32_t add_entry(const SpriteEntry& entry);

    void submit(rhi::CommandContext& commandContext, const DrawParams& drawParams, uint32_t entryIndex);

    void flush(rhi::CommandContext& commandContext);

private:
    std::shared_ptr<SpriteCache> m_spriteCache;
    std::vector<SpriteEntry> m_entries;
    DrawBatch m_batch;
};

void add_sprite_draw_entries(SpriteContext& spriteContext, DrawContext& drawContext, objects::Objects& objects);

}

#endif //DUK_RENDERER_SPRITE_DRAW_H
