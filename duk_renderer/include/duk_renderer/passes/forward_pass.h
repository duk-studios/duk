/// 05/10/2023
/// forward_pass.h

#ifndef DUK_RENDERER_FORWARD_PASS_H
#define DUK_RENDERER_FORWARD_PASS_H

#include <duk_renderer/material/draw_entry.h>
#include <duk_renderer/passes/pass.h>

namespace duk::renderer {

class SpriteCache;
class Renderer;

struct DrawDataGroup {
    typedef SortKey (*CalculateSortKeyFunc)(const duk::objects::Object& object);

    std::vector<ObjectEntry> objectEntries;
    std::vector<uint16_t> sortedIndices;
    std::vector<DrawEntry> drawEntries;
    CalculateSortKeyFunc calculateSortKey;

    void clear();
};

struct DrawData {
    DrawDataGroup opaqueGroup;
    DrawDataGroup transparentGroup;

    void clear();
};

struct ForwardPassCreateInfo {
    Renderer* renderer;
};

class ForwardPass : public Pass {
public:
    explicit ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo);

    ~ForwardPass() override;

    void update(const UpdateParams& params) override;

    void render(duk::rhi::CommandBuffer* commandBuffer) override;

    DUK_NO_DISCARD PassConnection* out_color();

private:
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Image> m_colorImage;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    PassConnection m_outColor;
    DrawData m_drawData;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_FORWARD_PASS_H
