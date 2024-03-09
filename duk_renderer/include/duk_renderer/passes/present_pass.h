/// 05/10/2023
/// present_pass.h

#ifndef DUK_RENDERER_PRESENT_PASS_H
#define DUK_RENDERER_PRESENT_PASS_H

#include <duk_renderer/brushes/generic_brush.h>
#include <duk_renderer/passes/pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_material.h>
#include <duk_renderer/resources/materials/material.h>

namespace duk::renderer {

struct PresentPassCreateInfo {
    Renderer* renderer;
    duk::platform::Window* window;
};

class PresentPass : public Pass {
public:
    explicit PresentPass(const PresentPassCreateInfo& presentPassCreateInfo);

    ~PresentPass() override;

    void update(const UpdateParams& params) override;

    void render(duk::rhi::CommandBuffer* commandBuffer) override;

    PassConnection* in_color();

private:
    Renderer* m_renderer;
    duk::event::Listener m_listener;
    PassConnection m_inColor;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    std::unique_ptr<Material> m_fullscreenMaterial;
    FullscreenMaterialDescriptorSet* m_fullscreenMaterialDescriptorSet;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_PRESENT_PASS_H
