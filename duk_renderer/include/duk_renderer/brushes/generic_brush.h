/// 11/10/2023
/// custom_brush.h

#ifndef DUK_RENDERER_GENERIC_BRUSH_H
#define DUK_RENDERER_GENERIC_BRUSH_H

#include <duk_renderer/brushes/brush.h>

namespace duk::renderer {

class GenericBrush : public Brush {
public:
    using DrawFunction = std::function<void(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance)>;

public:
    explicit GenericBrush(DrawFunction drawFunction);

    ~GenericBrush() override;

    void draw(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) override;

private:
    DrawFunction m_drawFunction;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_GENERIC_BRUSH_H
