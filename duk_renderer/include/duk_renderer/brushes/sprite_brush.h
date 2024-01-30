//
// Created by rov on 1/9/2024.
//

#ifndef DUK_RENDERER_SPRITE_BRUSH_H
#define DUK_RENDERER_SPRITE_BRUSH_H

#include <duk_renderer/brushes/brush.h>
#include <duk_renderer/resources/sprite.h>
#include <duk_renderer/vertex_types.h>
#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct SpriteBrushCreateInfo {
    Renderer* renderer;
    uint32_t initialSpriteCapacity;
};

class SpriteBrush : public Brush {
public:

    explicit SpriteBrush(const SpriteBrushCreateInfo& spriteBrushCreateInfo);

    void push(const Sprite* sprite, const glm::mat4& model);

    void clear();

    void resize(uint32_t spriteCapacity);

    void draw(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) override;

private:
    VertexBufferArray m_vertexBuffers;
    std::shared_ptr<duk::rhi::Buffer> m_indexBuffer;
    uint32_t m_spriteCount;
    uint32_t m_spriteCapacity;
};

struct SpriteBrushPoolCreateInfo {
    Renderer* renderer;
};

class SpriteBrushPool {
public:

    explicit SpriteBrushPool(const SpriteBrushPoolCreateInfo& spriteBrushPoolCreateInfo);

    DUK_NO_DISCARD SpriteBrush* get(uint32_t spriteHandle);

    void clear();

private:
    Renderer* m_renderer;
    std::vector<std::unique_ptr<SpriteBrush>> m_spriteBrushes;
    std::unordered_map<uint32_t, size_t> m_spriteHandleToIndex;
    size_t m_spriteBrushesInUse;
};

}

#endif //DUK_RENDERER_SPRITE_BRUSH_H
