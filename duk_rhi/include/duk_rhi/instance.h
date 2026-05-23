//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_INSTANCE_H
#define DUK_RHI_INSTANCE_H

#include <duk_rhi/command_context.h>
#include <duk_rhi/command_queue.h>
#include <duk_rhi/capabilities.h>
#include <duk_rhi/pixel.h>

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

struct InstanceCreateInfo {
    duk::log::Logger* logger;
    const char* applicationName;
    uint32_t applicationVersion;
    const char* engineName;
    uint32_t engineVersion;
    API api;
    bool validationLayers;
    size_t deviceIndex;
};

struct CommandContextCreateInfo {
    CommandQueue::Type::Mask type{CommandQueue::Type::GRAPHICS};
    const platform::Window* window{nullptr};
    uint32_t framesInFlight{2};
    PixelFormat depthFormat{PixelFormat::UNDEFINED};
};

class Instance {
public:
    static std::shared_ptr<Instance> create(const InstanceCreateInfo& instanceCreateInfo);

    virtual ~Instance() = default;

    /// Returns the capabilities of this device, populated at creation time.
    DUK_NO_DISCARD virtual const Capabilities& capabilities() const = 0;

    DUK_NO_DISCARD virtual std::unique_ptr<CommandContext> create_command_context(const CommandContextCreateInfo& commandContextCreateInfo) = 0;

    DUK_NO_DISCARD virtual std::unique_ptr<CommandQueue> create_command_queue(const CommandContextCreateInfo& commandContextCreateInfo) = 0;
};

}// namespace duk::rhi

#endif//DUK_RHI_INSTANCE_H
