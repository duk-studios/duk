/// 01/08/2023
/// forward_renderer.h

#ifndef DUK_RENDERER_FORWARD_RENDERER_H
#define DUK_RENDERER_FORWARD_RENDERER_H

#include <duk_renderer/renderer.h>
#include <duk_rhi/rhi.h>
#include <duk_scene/objects.h>

namespace duk::renderer {

struct ForwardRendererCreateInfo {
    RendererCreateInfo rendererCreateInfo;
};

class ForwardRenderer : public Renderer {
public:
    explicit ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo);
};

}// namespace duk::renderer

#endif// DUK_RENDERER_FORWARD_RENDERER_H
