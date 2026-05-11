//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_RENDERER_H
#define DUK_RHI_RENDERER_H

#include <duk_rhi/command_context.h>
#include <duk_rhi/command_queue.h>
#include <duk_rhi/rhi_capabilities.h>

#include <duk_log/logger.h>

#include <memory>

namespace duk::platform {

class Window;

}

namespace duk::rhi {

enum class API {
    UNDEFINED,
    VULKAN,
    OPENGL45,
    DX12
};

struct RHICreateInfo {
    duk::log::Logger* logger;
    const char* applicationName;
    uint32_t applicationVersion;
    const char* engineName;
    uint32_t engineVersion;
    API api;
    bool validationLayers;
    size_t deviceIndex;
    uint32_t framesInFlight;
};

class RHI {
public:
    static std::shared_ptr<RHI> create_rhi(const RHICreateInfo& rhiCreateInfo);

    virtual ~RHI() = default;

    /// Returns an object that can be used to check the limits and capabilities of this device.
    DUK_NO_DISCARD virtual RHICapabilities* capabilities() const = 0;

    struct CommandQueueCreateInfo {
        CommandQueue::Type::Mask type;
        const platform::Window* window;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<CommandQueue> create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) = 0;
};

}// namespace duk::rhi

#endif//DUK_RHI_RENDERER_H
