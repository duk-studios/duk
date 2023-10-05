/// 01/08/2023
/// forward_renderer.cpp

#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/passes/forward_pass.h>

namespace duk::renderer {

ForwardRenderer::ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo) :
    Renderer(forwardRendererCreateInfo.rendererCreateInfo) {

    {
        ForwardPassCreateInfo forwardPassCreateInfo = {};
        forwardPassCreateInfo.renderer = this;

        m_passes.push_back(std::make_shared<ForwardPass>(forwardPassCreateInfo));
    }

}

}
