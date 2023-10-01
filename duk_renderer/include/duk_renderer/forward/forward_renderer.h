/// 01/08/2023
/// forward_renderer.h

#ifndef DUK_RENDERER_FORWARD_RENDERER_H
#define DUK_RENDERER_FORWARD_RENDERER_H

#include <duk_scene/scene.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/canvas.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct ForwardRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
};

class ForwardRenderer : public Renderer {
public:

    explicit ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo);

    void render(duk::scene::Scene* scene) override;

    void resize(uint32_t width, uint32_t height) override;

private:
    duk::tools::FixedVector<ObjectEntry, scene::MAX_OBJECTS> m_objectEntries;
    duk::tools::FixedVector<uint16_t, scene::MAX_OBJECTS> m_sortedObjectIndices;
    duk::tools::FixedVector<PaintEntry, scene::MAX_OBJECTS> m_paintEntries;
    std::unique_ptr<Canvas> m_canvas;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
};

}

#endif // DUK_RENDERER_FORWARD_RENDERER_H

