/// 05/10/2023
/// forward_pass.h

#ifndef DUK_RENDERER_FORWARD_PASS_H
#define DUK_RENDERER_FORWARD_PASS_H

#include <duk_renderer/passes/pass.h>
#include <duk_renderer/renderer.h>

#include <duk_tools/fixed_vector.h>

namespace duk::renderer {

struct ForwardPassCreateInfo {
    Renderer* renderer;
};

class ForwardPass : public Pass {
public:

    explicit ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo);

    ~ForwardPass() override;

    void render(const RenderParams& renderParams) override;

private:
    duk::tools::FixedVector<Renderer::ObjectEntry, scene::MAX_OBJECTS> m_objectEntries;
    duk::tools::FixedVector<uint16_t, scene::MAX_OBJECTS> m_sortedObjectIndices;
    duk::tools::FixedVector<Renderer::PaintEntry, scene::MAX_OBJECTS> m_paintEntries;
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
};

}

#endif // DUK_RENDERER_FORWARD_PASS_H

