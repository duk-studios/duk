/// 05/10/2023
/// present_pass.h

#ifndef DUK_RENDERER_PRESENT_PASS_H
#define DUK_RENDERER_PRESENT_PASS_H

#include <duk_renderer/material/material.h>
#include <duk_renderer/passes/pass.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

class ShaderPipelinePool;

struct PresentPassCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    duk::platform::Window* window;
    duk::resource::Handle<ShaderPipeline> shader;
};

class PresentPass : public Pass {
public:
    explicit PresentPass(const PresentPassCreateInfo& presentPassCreateInfo);

    ~PresentPass() override;

    void update(const UpdateParams& params) override;

    void render(duk::rhi::CommandBuffer* commandBuffer) override;

    PassConnection* in_color();

private:
    duk::rhi::RHI* m_rhi;
    PassConnection m_inColor;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    std::unique_ptr<Material> m_fullscreenMaterial;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_PRESENT_PASS_H
