/// 01/08/2023
/// forward_renderer.cpp

#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/passes/present_pass.h>

namespace duk::renderer {

ForwardRenderer::ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo)
    : Renderer(forwardRendererCreateInfo.rendererCreateInfo) {
    ForwardPassCreateInfo forwardPassCreateInfo = {};
    forwardPassCreateInfo.renderer = this;

    auto forwardPass = std::make_shared<ForwardPass>(forwardPassCreateInfo);

    PresentPassCreateInfo presentPassCreateInfo = {};
    presentPassCreateInfo.renderer = this;

    auto presentPass = std::make_shared<PresentPass>(presentPassCreateInfo);

    forwardPass->out_color()->connect(presentPass->in_color());

    m_passes.push_back(forwardPass);
    m_passes.push_back(presentPass);
}

}// namespace duk::renderer
