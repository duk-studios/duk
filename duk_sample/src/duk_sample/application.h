/// 03/06/2023
/// application.h

#ifndef DUK_SAMPLE_APPLICATION_H
#define DUK_SAMPLE_APPLICATION_H

#include <duk_log/logger.h>
#include <duk_log/sink.h>
#include <duk_platform/window.h>
#include <duk_rhi/rhi.h>
#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/materials/color/color_material.h>
#include <duk_renderer/materials/phong/phong_material.h>

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

private:
    duk::log::Logger m_logger;
    duk::events::EventListener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::renderer::ForwardRenderer> m_renderer;
    std::shared_ptr<duk::renderer::MeshBufferPool> m_meshBufferPool;
    std::shared_ptr<duk::renderer::Mesh> m_quadMesh;
    std::shared_ptr<duk::renderer::Mesh> m_cubeMesh;
    std::shared_ptr<duk::renderer::Mesh> m_sphereMesh;
    std::shared_ptr<duk::renderer::Painter> m_colorPainter;
    std::shared_ptr<duk::renderer::ColorMaterial> m_colorMaterial;
    std::shared_ptr<duk::renderer::Painter> m_phongPainter;
    std::shared_ptr<duk::renderer::PhongMaterial> m_phongGreenMaterial, m_phongBlueMaterial, m_phongRedMaterial, m_phongWhiteMaterial, m_phongYellowMaterial, m_phongUnknownMaterial;
    std::shared_ptr<duk::scene::Scene> m_scene;
    duk::scene::Object m_camera;
    bool m_run;
};

}

#endif // DUK_SAMPLE_APPLICATION_H

