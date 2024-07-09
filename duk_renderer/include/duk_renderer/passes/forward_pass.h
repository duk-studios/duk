/// 05/10/2023
/// forward_pass.h

#ifndef DUK_RENDERER_FORWARD_PASS_H
#define DUK_RENDERER_FORWARD_PASS_H

#include <duk_renderer/material/draw_entry.h>
#include <duk_renderer/mesh/draw_buffer.h>
#include <duk_renderer/passes/pass.h>

namespace duk::renderer {

class SpriteCache;
class Renderer;

struct DrawGroupData {
    typedef SortKey (*CalculateSortKeyFunc)(const duk::objects::Object& object);

    std::vector<ObjectEntry> objectEntries;
    std::vector<uint16_t> sortedIndices;
    std::vector<DrawEntry> instanceDrawEntries;
    std::vector<IndirectDrawEntry> drawEntries;
    CalculateSortKeyFunc calculateSortKey;

    void clear();
};

struct DrawData {
    DrawGroupData opaqueGroup;
    DrawGroupData transparentGroup;
    std::unique_ptr<DrawBuffer> drawBuffer;

    void clear();
};

struct ForwardPassCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class ForwardPass : public Pass {
public:
    explicit ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo);

    ~ForwardPass() override;

    void update(const UpdateParams& params) override;

    void render(duk::rhi::CommandBuffer* commandBuffer) override;

    DUK_NO_DISCARD PassConnection* out_color();

private:
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    std::shared_ptr<duk::rhi::Image> m_colorImage;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    PassConnection m_outColor;
    DrawData m_drawData;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_FORWARD_PASS_H
