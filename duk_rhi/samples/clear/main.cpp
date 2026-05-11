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

#include <duk_rhi/rhi.h>
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
    duk::rhi::RHICreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName = "clear_screen_sample";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.engineName = "duk";
    rhiCreateInfo.engineVersion = 1;
    rhiCreateInfo.api = duk::rhi::API::VULKAN;
    rhiCreateInfo.validationLayers = true;
    rhiCreateInfo.deviceIndex = 0;
    rhiCreateInfo.framesInFlight = 2;
    rhiCreateInfo.logger = duk::log::instance()->default_logger();

    auto rhi = duk::rhi::RHI::create_rhi(rhiCreateInfo);

    // -----------------------------------------------------------------------
    // Command queue and shared context
    // -----------------------------------------------------------------------
    duk::rhi::RHI::CommandQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.type = duk::rhi::CommandQueue::Type::GRAPHICS;
    queueCreateInfo.window = window.get();
    auto queue = rhi->create_command_queue(queueCreateInfo);

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    window->show();

    while (running) {
        platform->pool_events();

        if (window->minimized()) {
            continue;
        }

        // Record and submit render commands.
        queue->submit([&](duk::rhi::CommandContext* ctx) {
            ctx->update();

            duk::rhi::CommandContext::RenderBeginParams renderBeginParams;
            renderBeginParams.clearColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);
            renderBeginParams.loadOp = duk::rhi::LoadOp::CLEAR;
            renderBeginParams.storeOp = duk::rhi::StoreOp::STORE;

            ctx->render_begin(renderBeginParams);

            ctx->render_end();

            ctx->flush();
        }).wait();
    }

    return 0;
}

