//
// Created by Ricardo on 19/08/2023.
//

#include <duk_log/log.h>
#include <duk_objects/objects.h>
#include <duk_platform/window.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/shader/shader_pipeline_pool.h>
#include <duk_renderer/image/image_pool.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/material/material_pool.h>
#include <duk_renderer/mesh/mesh_pool.h>
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
        rendererCreateInfo.pools = &m_pools;
        rendererCreateInfo.api = duk::rhi::API::VULKAN;
        rendererCreateInfo.logger = duk::log::Logging::instance(true)->default_logger();

        m_renderer = std::make_unique<duk::renderer::Renderer>(rendererCreateInfo);

        duk::renderer::add_forward_passes(m_renderer.get(), m_window.get());

        // mesh pool
        duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
        meshPoolCreateInfo.meshBufferPool = m_renderer->mesh_buffer_pool();
        m_pools.create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);

        // image pool
        duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
        imagePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

        // shader pool
        duk::renderer::ShaderPipelinePoolCreateInfo shaderPipelinePoolCreateInfo = {};
        shaderPipelinePoolCreateInfo.renderer = m_renderer.get();
        m_pools.create_pool<duk::renderer::ShaderPipelinePool>(shaderPipelinePoolCreateInfo);

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

    auto add_mesh_object = [](duk::objects::Objects& objects, const duk::renderer::MeshResource& mesh, const duk::renderer::MaterialResource& material, const glm::vec3& position) -> duk::objects::Object {
        duk::objects::Object object = objects.add_object();
        auto meshSlot = object.add<duk::renderer::MeshSlot>();
        meshSlot->mesh = mesh;

        auto materialSlot = object.add<duk::renderer::MaterialSlot>();
        materialSlot->material = material;

        auto transform = object.add<duk::renderer::Transform>();
        transform->position = position;
        transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
        transform->rotation = glm::radians(glm::vec3(30, 45, 0));

        return object;
    };

    auto materialPool = application.pools()->get<duk::renderer::MaterialPool>();

    // Create a phong material
    auto material = duk::renderer::create_phong_material(application.renderer());
    // material->set("uProperties", "color", glm::vec4(1.0f, 0.5f, 0.8f, 1.0f));
    // material->set("uBaseColor", imagePool->white_image(), {duk::rhi::Sampler::Filter::NEAREST, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE});

    auto mesh = application.pools()->get<duk::renderer::MeshPool>()->cube();

    auto cube1 = add_mesh_object(objects, mesh, material, glm::vec3(0, 2, -10));
    auto cube2 = add_mesh_object(objects, mesh, material, glm::vec3(0, -2, -10));

    auto cube1Transform = cube1.component<duk::renderer::Transform>();
    auto cube2Transform = cube2.component<duk::renderer::Transform>();

    material->set(cube1.id(), "uProperties", "color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    material->set(cube2.id(), "uProperties", "color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

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

        auto time = timer.time();

        //Rotating the cube in the X value by the timer total duration.
        cube1Transform->rotation = glm::radians(glm::vec3(time * 30, 45.0f, 0.0f));

        cube2Transform->rotation = glm::radians(glm::vec3(time * -52, sinf(time * 4) * 85, 15.0f));

        duk::renderer::update_transforms(objects);
        duk::renderer::update_cameras(objects, application.renderer()->render_height(), application.renderer()->render_height());

        //Telling to our renderer to render the objects we want.
        application.renderer()->render(objects, *application.pools());

        timer.stop();
    }

    return 0;
}
