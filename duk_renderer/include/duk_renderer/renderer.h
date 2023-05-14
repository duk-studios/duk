//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_renderer/renderer_error.h>

#include <tl/expected.hpp>

#include <memory>

namespace duk::renderer {

enum class RendererAPI {
    UNDEFINED,
    VULKAN,
    OPENGL45,
    DX12
};

struct RendererCreateInfo {
    RendererAPI api;
};

class Renderer {
public:

    using ExpectedRenderer = tl::expected<std::shared_ptr<Renderer>, RendererError>;

public:

    static ExpectedRenderer create_renderer(const RendererCreateInfo& rendererCreateInfo);

    /// destructor
    virtual ~Renderer() = default;

    /// called to start a new frame
    virtual void begin_frame() = 0;

    /// called to end a frame, TODO: not sure if it will already present the frame
    virtual void end_frame() = 0;

private:

};

}

#endif //DUK_RENDERER_RENDERER_H
