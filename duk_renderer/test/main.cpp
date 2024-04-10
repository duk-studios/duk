//
// Created by Ricardo on 19/08/2023.
//

#include <duk_log/log.h>
#include <duk_objects/objects.h>
#include <duk_platform/window.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/image/image_pool.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/material/material_pool.h>
#include <duk_renderer/mesh/mesh_pool.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/rhi.h>
#include <duk_tools/timer.h>
#include <iostream>

class Application {
public:
    Application() {
        duk::platform::WindowCreateInfo windowCreateInfo = {"RendererWindow", 640, 720};

        //The result of window creation is checked, and if successful, the program proceeds
        m_window = duk::platform::Window::create_window(windowCreateInfo);
        std::cout << "Window creation succeed!" << std::endl;
        m_run = true;

        //Event listeners are set up to respond to window close and destroy event.
        m_listener.listen(m_window->window_close_event, [this] {
            std::cout << "The window is closed!" << std::endl;
            m_window->close();
        });

        m_listener.listen(m_window->window_destroy_event, [this]() {
            m_run = false;
        });

        // Create renderer
        duk::renderer::RendererCreateInfo rendererCreateInfo = {};
        rendererCreateInfo.window = m_window.get();
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.logger = duk::log::Logging::instance(true)->default_logger();

        m_renderer = duk::renderer::make_forward_renderer(rendererCreateInfo);

        // mesh pool
        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);

        // image pool
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

        // material pool
        duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
        materialPoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);
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
    Application application;

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

    //And finally adding the cube to the objects.
    //Our cube has the Mesh Drawing component, that specifies what mesh type and material we are going to use.
    //Also our position and scale and rotation values.
    duk::objects::Object cubeObject = objects.add_object();
    cubeObject.add<duk::renderer::Transform>();
    auto cubeMeshRenderer = cubeObject.add<duk::renderer::MeshRenderer>();
    cubeMeshRenderer->mesh = application.pools()->get<duk::renderer::MeshPool>()->cube();

    auto imagePool = application.pools()->get<duk::renderer::ImagePool>();
    auto materialPool = application.pools()->get<duk::renderer::MaterialPool>();

    // Create a phong material
    duk::renderer::PhongMaterialDataSource phongMaterialDataSource = {};
    phongMaterialDataSource.albedo = glm::vec4(0.4f, 1.0f, 0.8f, 1.0f);
    phongMaterialDataSource.albedoTexture = imagePool->white_image();
    phongMaterialDataSource.specular = 32;
    phongMaterialDataSource.specularTexture = imagePool->white_image();
    phongMaterialDataSource.update_hash();
    cubeMeshRenderer->material = materialPool->create(duk::resource::Id(666), &phongMaterialDataSource);
    auto cubeTransform = cubeObject.add<duk::renderer::Transform>();
    cubeTransform->position = glm::vec3(0, 0, -10);
    cubeTransform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    cubeTransform->rotation = glm::radians(glm::vec3(30, 45, 0));

    //Show the window we created.
    application.window()->show();

    //Creating a timer
    duk::tools::Timer timer;

    //Our main loop
    while (application.run()) {
        timer.start();

        application.window()->pool_events();

        while (application.window()->minimized()) {
            application.window()->wait_events();
        }

        duk::renderer::update_transforms(objects);
        duk::renderer::update_cameras(objects, application.renderer()->render_height(), application.renderer()->render_height());

        //Rotating the cube in the X value by the timer total duration.
        cubeTransform->rotation = glm::vec3(timer.total_duration().count(), 45.0f, 0.0f);

        //Telling to our renderer to render the objects we want.
        application.renderer()->render(objects);

        timer.stop();
    }

    return 0;
}
