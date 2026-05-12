//
// duk_rhi sample: clear screen
//
// Demonstrates the minimal frame loop using the new RHI API:
//   RHI (device) + Surface (window) + CommandContext (recording + resources)
//   + CommandQueue (submission).
//
// Every frame the swapchain image is cleared to a solid colour via a
// render pass with a clear value — no shaders, no geometry.
//

#include <duk_log/logger.h>

#include <duk_platform/platform.h>

#include <duk_rhi/instance.h>
#include <duk_log/log.h>
#include <duk_log/sinks/term_color_sink.h>


int main() {

    // -----------------------------------------------------------------------
    // Window
    // -----------------------------------------------------------------------
    auto platform = duk::platform::create_platform();

    duk::log::instance()->add_sink(std::make_unique<duk::log::TermColorSink>("duk", duk::log::Level::INFO));

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title = "duk_rhi - clear";
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;
    windowCreateInfo.style = duk::platform::WindowStyle::STANDARD;

    auto window = platform->create_window(windowCreateInfo);

    volatile bool running = true;

    duk::event::Listener listener;
    listener.listen(window->window_close_event, [&window] { window->close(); });
    listener.listen(window->window_destroy_event, [&running] { running = false; });
    listener.listen(window->key_event, [&window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask, duk::platform::KeyAction action) {
        if (key == duk::platform::Keys::ESC && action == duk::platform::KeyAction::PRESS) {
            window->close();
        }
    });

    // -----------------------------------------------------------------------
    // RHI device (no window attached)
    // -----------------------------------------------------------------------
    duk::rhi::InstanceCreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName = "clear_screen_sample";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.engineName = "duk";
    rhiCreateInfo.engineVersion = 1;
    rhiCreateInfo.api = duk::rhi::API::VULKAN;
    rhiCreateInfo.validationLayers = true;
    rhiCreateInfo.deviceIndex = 0;

    rhiCreateInfo.logger = duk::log::instance()->default_logger();

    auto rhi = duk::rhi::Instance::create(rhiCreateInfo);

    // -----------------------------------------------------------------------
    // Command context
    // -----------------------------------------------------------------------
    duk::rhi::CommandContextCreateInfo contextCreateInfo = {};
    contextCreateInfo.type = duk::rhi::CommandQueue::Type::GRAPHICS;
    contextCreateInfo.window = window.get();
    contextCreateInfo.framesInFlight = 2;
    auto ctx = rhi->create_command_context(contextCreateInfo);

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    window->show();

    while (running) {
        platform->pool_events();

        if (window->minimized() || !window->valid()) {
            continue;
        }

        // Record and submit render commands.
        ctx->prepare();

        duk::rhi::RenderBeginParams renderBeginParams;
        renderBeginParams.clearColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);
        renderBeginParams.loadOp = duk::rhi::LoadOp::CLEAR;
        renderBeginParams.storeOp = duk::rhi::StoreOp::STORE;

        auto render = ctx->render(renderBeginParams);

        ctx->flush();
    }

    return 0;
}

