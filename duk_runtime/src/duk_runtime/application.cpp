//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_api/register_types.h>
#include <duk_api/api.h>

#include <duk_serial/json/serializer.h>

namespace duk::runtime {

using FrameDuration = std::chrono::duration<float, std::ratio<1, 144>>;

namespace detail {

static duk::resource::LoadMode load_mode() {
#ifdef DUK_PACK
    return duk::resource::LoadMode::PACKED;
#else
    return duk::resource::LoadMode::UNPACKED;
#endif
}

static duk::engine::Settings load_settings() {
    auto loadMode = load_mode();
    std::string settingsJson;
    switch (loadMode) {
        case resource::LoadMode::UNPACKED:
            settingsJson = duk::tools::load_text("settings.json");
            break;
        case resource::LoadMode::PACKED:
            settingsJson = duk::tools::load_compressed_text("settings.bin");
            break;
    }
    auto settings = duk::serial::read_json<duk::engine::Settings>(settingsJson);
    settings.loadMode = loadMode;
    return settings;
}

}// namespace detail

Application::Application(const ApplicationCreateInfo& applicationCreateInfo)
    : m_platform(applicationCreateInfo.platform)
    , m_run(false) {
    DUK_ASSERT(m_platform);
    duk::api::register_types();

    auto settings = detail::load_settings();

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = settings.name.c_str();
    windowCreateInfo.width = settings.resolution.x;
    windowCreateInfo.height = settings.resolution.y;

    m_window = m_platform->create_window(windowCreateInfo);

    duk::engine::ContextCreateInfo contextCreateInfo = {};
    contextCreateInfo.workingDirectory = ".";
    contextCreateInfo.settings = detail::load_settings();
    contextCreateInfo.platform = m_platform;
    contextCreateInfo.window = m_window.get();
    contextCreateInfo.rendererApiValidationLayers = applicationCreateInfo.rendererApiValidationLayers;

    m_context = std::make_unique<duk::engine::Context>(contextCreateInfo);

    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this] {
        stop();
    });
}

Application::~Application() = default;

void Application::run() {
    duk_api_run_enter(*m_context->globals());

    m_window->show();

    loop_sleep_excess();

    duk_api_run_exit(*m_context->globals());
}

void Application::stop() {
    m_run = false;
}

void Application::loop_dynamic_delta_time() {
    const auto input = m_context->input();
    const auto audio = m_context->audio();
    const auto director = m_context->director();
    const auto renderer = m_context->renderer();
    const auto globals = m_context->globals();
    const auto timer = m_context->timer();
    m_run = true;
    while (m_run) {
        timer->tick();
        while (m_window->valid() && m_window->minimized()) {
            m_platform->wait_events();
        }

        if (!m_window->valid()) {
            continue;
        }

        m_platform->pool_events();

        input->refresh();

        audio->update();

        director->update(*globals);

        renderer->render(director->scene()->objects());
    }
}

void Application::loop_sleep_excess() {
    const auto input = m_context->input();
    const auto audio = m_context->audio();
    const auto director = m_context->director();
    const auto renderer = m_context->renderer();
    const auto globals = m_context->globals();
    const auto timer = m_context->timer();
    m_run = true;
    while (m_run) {
        while (m_window->valid() && m_window->minimized()) {
            m_platform->wait_events();
        }

        if (!m_window->valid()) {
            continue;
        }

        const auto start = timer->now();

        input->refresh();

        m_platform->pool_events();

        audio->update();

        director->update(*globals);

        renderer->render(director->scene()->objects());
        auto end = timer->now();
        const auto workingTime = end - start;
        const auto sleepTime = std::chrono::duration_cast<std::chrono::nanoseconds>(FrameDuration(1) - workingTime);
        const auto awakeTime = end + sleepTime;
        while (timer->now() < awakeTime) {
            std::this_thread::yield();
        }
        const auto realElapsed = timer->now() - start;
        if (sleepTime > std::chrono::nanoseconds(0)) {
            timer->add_duration(FrameDuration(1));
        } else {
            timer->add_duration(realElapsed);
            duk::log::warn("Frame rate falling behind, elapsed time was {0}ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(realElapsed).count());
        }
    }
}

}// namespace duk::runtime
