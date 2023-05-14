//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_renderer/renderer_error.h>
#include <duk_renderer/command_queue.h>

#include <duk_log/logger.h>

#include <tl/expected.hpp>

#include <memory>

namespace duk::platform {

class Window;

}

namespace duk::renderer {

enum class RendererAPI {
    UNDEFINED,
    VULKAN,
    OPENGL45,
    DX12
};

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    const char* applicationName;
    uint32_t applicationVersion;
    const char* engineName;
    uint32_t engineVersion;
    RendererAPI api;
    size_t deviceIndex;
};

class Renderer;
using ExpectedRenderer = tl::expected<std::shared_ptr<Renderer>, RendererError>;
using ExpectedCommandQueue = tl::expected<std::shared_ptr<CommandQueue>, RendererError>;

class Renderer {
public:

    static ExpectedRenderer create_renderer(const RendererCreateInfo& rendererCreateInfo);

    /// destructor
    virtual ~Renderer() = default;

    /// called to start a new frame
    virtual void begin_frame() = 0;

    /// called to end a frame, TODO: not sure if it will already present the frame
    virtual void end_frame() = 0;

    virtual ExpectedCommandQueue create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) = 0;

private:

};

}

#endif //DUK_RENDERER_RENDERER_H
