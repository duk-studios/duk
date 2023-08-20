/// 01/08/2023
/// forward_renderer.h

#ifndef DUK_RENDERER_FORWARD_RENDERER_H
#define DUK_RENDERER_FORWARD_RENDERER_H

#include <duk_scene/scene.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct ForwardRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
};

class ForwardRenderer : public Renderer {
public:

    explicit ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo);

    void render(duk::scene::Scene* scene) override;

private:
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
};

}

#endif // DUK_RENDERER_FORWARD_RENDERER_H

