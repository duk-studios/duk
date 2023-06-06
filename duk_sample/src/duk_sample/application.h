/// 03/06/2023
/// application.h

#ifndef DUK_SAMPLE_APPLICATION_H
#define DUK_SAMPLE_APPLICATION_H

#include <duk_log/logger.h>
#include <duk_log/sink.h>
#include <duk_platform/window.h>
#include <duk_renderer/renderer.h>

#include <memory>

namespace duk::sample {

struct ApplicationCreateInfo {
    duk::log::Sink* sink;
    const char* name;
};

class Application {
public:

    explicit Application(const ApplicationCreateInfo& applicationCreateInfo);

    ~Application();

    void run();

private:

    template<typename T>
    auto check_expected_result(T&& expectedResult) {
        if (!expectedResult) {
            auto errorDescription = expectedResult.error().description();
            m_logger.log_fatal() << "Unexpected result: " << errorDescription;
            std::terminate();
        }
        return std::move(expectedResult.value());
    }

    void update(double time, double deltaTime);

    void draw();

    duk::renderer::FutureCommand main_render_pass();

private:
    duk::log::Logger m_logger;
    duk::events::EventListener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::shared_ptr<duk::renderer::Renderer> m_renderer;
    std::shared_ptr<duk::renderer::CommandQueue> m_mainCommandQueue;
    std::shared_ptr<duk::renderer::CommandScheduler> m_scheduler;
    std::shared_ptr<duk::renderer::RenderPass> m_renderPass;
    std::shared_ptr<duk::renderer::FrameBuffer> m_frameBuffer;
    std::shared_ptr<duk::renderer::Shader> m_colorShader;
    std::shared_ptr<duk::renderer::Pipeline> m_pipeline;
    std::shared_ptr<duk::renderer::Buffer> m_vertexBuffer;
    std::shared_ptr<duk::renderer::Buffer> m_indexBuffer;
    std::shared_ptr<duk::renderer::Buffer> m_uniformBuffer;
    std::shared_ptr<duk::renderer::Image> m_image;
    std::shared_ptr<duk::renderer::DescriptorSet> m_descriptorSet;
    bool m_run;

};


}

#endif // DUK_SAMPLE_APPLICATION_H

