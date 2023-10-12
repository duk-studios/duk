/// 05/10/2023
/// present_pass.h

#ifndef DUK_RENDERER_PRESENT_PASS_H
#define DUK_RENDERER_PRESENT_PASS_H

#include <duk_renderer/passes/pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/brushes/generic_brush.h>

namespace duk::renderer {

class FullscreenPainter;
class FullscreenPalette;

struct PresentPassCreateInfo {
    Renderer* renderer;
};

class PresentPass : public Pass {
public:

    explicit PresentPass(const PresentPassCreateInfo& presentPassCreateInfo);

    ~PresentPass() override;

    void render(const RenderParams& renderParams) override;

    PassConnection* in_color();

private:
    Renderer* m_renderer;
    GenericBrush m_fullscreenTriangleBrush;
    PassConnection m_inColor;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    std::unique_ptr<FullscreenPainter> m_fullscreenPainter;
    std::shared_ptr<FullscreenPalette> m_fullscreenPalette;
};

}

#endif // DUK_RENDERER_PRESENT_PASS_H

