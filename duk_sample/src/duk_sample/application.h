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

    static std::vector<uint8_t> load_bytes(const char* filepath);

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

    duk::renderer::FutureCommand compute_pass();

    duk::renderer::FutureCommand reacquire_compute_resources();

private:
    duk::log::Logger m_logger;
    duk::events::EventListener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::shared_ptr<duk::renderer::Renderer> m_renderer;
    std::shared_ptr<duk::renderer::CommandQueue> m_mainCommandQueue;
    std::shared_ptr<duk::renderer::CommandQueue> m_computeQueue;
    std::shared_ptr<duk::renderer::CommandScheduler> m_scheduler;
    std::shared_ptr<duk::renderer::RenderPass> m_renderPass;
    std::shared_ptr<duk::renderer::FrameBuffer> m_frameBuffer;
    std::shared_ptr<duk::renderer::Shader> m_colorShader;
    std::shared_ptr<duk::renderer::Shader> m_computeShader;
    std::shared_ptr<duk::renderer::GraphicsPipeline> m_graphicsPipeline;
    std::shared_ptr<duk::renderer::ComputePipeline> m_computePipeline;
    std::shared_ptr<duk::renderer::Buffer> m_vertexBuffer;
    std::shared_ptr<duk::renderer::Buffer> m_indexBuffer;
    std::shared_ptr<duk::renderer::Buffer> m_materialUniformBuffer;
    std::shared_ptr<duk::renderer::Buffer> m_transformUniformBuffer;
    std::shared_ptr<duk::renderer::Image> m_image;
    std::shared_ptr<duk::renderer::Image> m_depthImage;
    std::shared_ptr<duk::renderer::DescriptorSet> m_descriptorSet;
    std::shared_ptr<duk::renderer::DescriptorSet> m_computeDescriptorSet;
    bool m_run;

};


}

#endif // DUK_SAMPLE_APPLICATION_H

