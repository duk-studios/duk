/// 01/08/2023
/// forward_renderer.h

#ifndef DUK_RENDERER_FORWARD_RENDERER_H
#define DUK_RENDERER_FORWARD_RENDERER_H

#include <duk_scene/objects.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct ForwardRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
};

class ForwardRenderer : public Renderer {
public:

    explicit ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo);

};

}

#endif // DUK_RENDERER_FORWARD_RENDERER_H

