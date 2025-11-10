//
// Created by Ricardo on 19/08/2023.
//

#include <duk_log/log.h>
#include <duk_objects/objects.h>
#include <duk_platform/window.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/shader/shader_pipeline_builtins.h>
#include <duk_renderer/image/image_builtins.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/mesh/mesh_builtins.h>
#include <duk_renderer/renderer.h>
#include <duk_tools/timer.h>
#include <duk_renderer/register_types.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>

#include <duk_platform/win32/platform_win_32.h>

class Application {
public:
    Application(duk::platform::Platform* platform) {
        duk::platform::WindowCreateInfo windowCreateInfo = {"RendererWindow", 640, 720};

        //The result of window creation is checked, and if successful, the program proceeds
        m_window = platform->create_window(windowCreateInfo);
        m_run = true;

        //Event listeners are set up to respond to window close and destroy event.
        m_listener.listen(m_window->window_close_event, [this] {
            m_window->close();
        });

        m_listener.listen(m_window->window_destroy_event, [this]() {
            m_run = false;
        });

        // Create renderer
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = m_window.get();
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.logger = duk::log::instance()->default_logger();
        rendererCreateInfo.renderSize = glm::vec2(640, 720);

        m_renderer = std::make_unique<duk::renderer::Renderer>(rendererCreateInfo);
    }

    duk::platform::Window* window() {
        return m_window.get();
    }

    duk::renderer::Renderer* renderer() {
        return m_renderer.get();
    }

    duk::resource::Pools* pools() {
        return &m_pools;
    }

    bool run() const {
        return m_run;
    }

private:
    duk::event::Listener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::shared_ptr<duk::renderer::Renderer> m_renderer;
    duk::resource::Pools m_pools;
    bool m_run;
};

int main() {
    duk::renderer::register_types();

    duk::platform::PlatformWin32CreateInfo platformWin32CreateInfo = {};
    platformWin32CreateInfo.instance = GetModuleHandle(NULL);

    duk::platform::PlatformWin32 platform(platformWin32CreateInfo);

    Application application(&platform);

    duk::objects::Objects objects;

    //Adding our first object to the objects, a camera.
    //Setup the camera position, and adding the Perspective Component to it.
    duk::objects::Object camera = objects.add_object();
    auto cameraTransform = camera.add<duk::renderer::Transform>();
    cameraTransform->position = glm::vec3(0, 0, 0);
    camera.add<duk::renderer::Camera>();
    auto cameraPerspective = camera.add<duk::renderer::PerspectiveCamera>();
    cameraPerspective->zFar = 1000.0f;
    cameraPerspective->zNear = 0.1f;
    cameraPerspective->fovDegrees = 60.0f;

    ////Adding a light to our objects.
    duk::objects::Object globalLight = objects.add_object();
    auto globalLightTransform = globalLight.add<duk::renderer::Transform>();
    globalLightTransform->position = glm::vec3(0, 4, -5);
    auto globalPointLight = globalLight.add<duk::renderer::PointLight>();
    globalPointLight->value.color = glm::vec3(1.0f, 1.0f, 1.0f);
    globalPointLight->value.intensity = 5.0f;

    //Show the window we created.
    application.window()->show();

    //Creating a timer
    duk::tools::Timer timer;

    //Our main loop
    while (application.run()) {
        timer.start();

        platform.pool_events();

        while (application.window()->minimized()) {
            platform.wait_events();
        }
        // duk::renderer::update_transforms(objects);
        duk::renderer::update_cameras(objects, application.renderer()->render_height(), application.renderer()->render_height());

        //Telling to our renderer to render the objects we want.
        application.renderer()->render(objects);

        timer.stop();
    }

    return 0;
}
