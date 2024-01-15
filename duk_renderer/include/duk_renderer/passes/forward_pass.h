/// 05/10/2023
/// forward_pass.h

#ifndef DUK_RENDERER_FORWARD_PASS_H
#define DUK_RENDERER_FORWARD_PASS_H

#include <duk_renderer/passes/pass.h>
#include <duk_renderer/materials/paint_entry.h>

#include <duk_tools/fixed_vector.h>

namespace duk::renderer {

class Renderer;

struct PaintData {
    duk::tools::FixedVector<ObjectEntry, scene::MAX_OBJECTS> objects;
    duk::tools::FixedVector<uint16_t, scene::MAX_OBJECTS> sortedObjects;
    duk::tools::FixedVector<PaintEntry, scene::MAX_OBJECTS> paintEntries;

    void clear();
};

struct ForwardPassCreateInfo {
    Renderer* renderer;
};

class ForwardPass : public Pass {
public:

    explicit ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo);

    ~ForwardPass() override;

    void render(const RenderParams& renderParams) override;

    DUK_NO_DISCARD PassConnection* out_color();

private:
    PaintData m_paintData;
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Image> m_colorImage;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    PassConnection m_outColor;
};

}

#endif // DUK_RENDERER_FORWARD_PASS_H

