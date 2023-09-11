/// 08/09/2023
/// canvas.h

#ifndef DUK_RENDERER_CANVAS_H
#define DUK_RENDERER_CANVAS_H

#include <duk_rhi/render_pass.h>
#include <duk_rhi/frame_buffer.h>

namespace duk::renderer {

struct CanvasCreateInfo {
    uint32_t width;
    uint32_t height;
    std::shared_ptr<duk::rhi::RenderPass> renderPass;
};

class Canvas {
public:

    explicit Canvas(const CanvasCreateInfo& canvasCreateInfo);

    DUK_NO_DISCARD uint32_t width() const;

    DUK_NO_DISCARD uint32_t height() const;

    DUK_NO_DISCARD duk::rhi::RenderPass* render_pass() const;

private:
    uint32_t m_width;
    uint32_t m_height;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;

};

}

#endif // DUK_RENDERER_CANVAS_H

