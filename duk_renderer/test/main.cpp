//
// Created by Ricardo on 19/08/2023.
//

#include <iostream>
#include <duk_rhi/rhi.h>
#include <duk_platform/window.h>
#include <duk_scene/scene.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/material_pool.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_tools/timer.h>
#include <duk_log/log.h>


int main() {
    bool run = false;

    //An event listener (duk::event::Listener) is created to handle window event.
    duk::event::Listener listener;

    //A Window is created with a specified name ("RendererWindow") and dimensions (640x720).
    std::shared_ptr<duk::platform::Window> window;
    duk::platform::WindowCreateInfo windowCreateInfo = {"RendererWindow", 640, 720};

    //The result of window creation is checked, and if successful, the program proceeds
    window = duk::platform::Window::create_window(windowCreateInfo);
    std::cout<< "Window creation succeed!" << std::endl;
    run = true;


    //Event listeners are set up to respond to window close and destroy event.
    listener.listen(window->window_close_event, [&window] {
        std::cout<<"The window is closed!" << std::endl;
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run](){
        run = false;
    });

    //A logger is created for debugging purposes.
    auto logger = duk::log::Logging::instance(true)->default_logger();

    //Setting up rendererCreateInfo.
    duk::renderer::RendererCreateInfo rendererCreateInfo = { };
    rendererCreateInfo.window = window.get();
    rendererCreateInfo.api = duk::rhi::API::VULKAN;
    rendererCreateInfo.logger = logger;

    //A forward renderer is created and associated with the window.
    duk::renderer::ForwardRendererCreateInfo forwardRendererCreateInfo;
    forwardRendererCreateInfo.rendererCreateInfo = rendererCreateInfo;

    //Creating our renderer with the forwardRendererCreateInfo, and a scene.
    auto renderer = std::make_unique<duk::renderer::ForwardRenderer>(forwardRendererCreateInfo);

    //Create a Pools object to hold our resources
    duk::resource::Pools pools;

    // mesh pool
    duk::renderer::MeshPoolCreateInfo meshPoolCreateInfo = {};
    meshPoolCreateInfo.renderer = renderer.get();
    auto meshPool = pools.create_pool<duk::renderer::MeshPool>(meshPoolCreateInfo);

    // image pool
    duk::renderer::ImagePoolCreateInfo imagePoolCreateInfo = {};
    imagePoolCreateInfo.renderer = renderer.get();
    auto imagePool = pools.create_pool<duk::renderer::ImagePool>(imagePoolCreateInfo);

    // material pool
    duk::renderer::MaterialPoolCreateInfo materialPoolCreateInfo = {};
    materialPoolCreateInfo.renderer = renderer.get();
    auto materialPool = pools.create_pool<duk::renderer::MaterialPool>(materialPoolCreateInfo);

    auto scene = std::make_unique<duk::scene::Scene>();
    auto& objects = scene->objects();

    //Adding our first object to the scene, a camera.
    //Setup the camera position, and adding the Perspective Component to it.
    duk::scene::Object camera = objects.add_object();
    auto cameraPosition = camera.add<duk::renderer::Position3D>();  
    cameraPosition->value = glm::vec3(0,0,0);
    camera.add<duk::renderer::Rotation3D>();
    auto cameraPerspective = camera.add<duk::renderer::PerspectiveCamera>();
    cameraPerspective->aspectRatio = (float)window->width() / (float)window->height();
    cameraPerspective->zFar = 1000.0f;
    cameraPerspective->zNear = 0.1f;
    cameraPerspective->fovDegrees = 60.0f;

    //An event listener is set up to adjust the camera's aspect ratio when the window is resized.
    listener.listen(window->window_resize_event, [cameraPerspective](uint32_t width, uint32_t height) {
        cameraPerspective->aspectRatio = (float)width / (float)height;
    });

    ////Adding a light to our scene.
    duk::scene::Object globalLight = objects.add_object();
    auto globalLightPosition = globalLight.add<duk::renderer::Position3D>();
    globalLightPosition->value = glm::vec3(0,4,-5);
    auto globalPointLight = globalLight.add<duk::renderer::PointLight>();
    globalPointLight->value.color = glm::vec3(1.0f,1.0f,1.0f);
    globalPointLight->value.intensity = 5.0f;

    //And finally adding the cube to the scene.
    //Our cube has the Mesh Drawing component, that specifies what mesh type and material we are going to use.
    //Also our position and scale and rotation values.
    duk::scene::Object cubeObject = objects.add_object();
    auto cubeMeshRenderer = cubeObject.add<duk::renderer::MeshRenderer>();
    cubeMeshRenderer->mesh = meshPool->cube();

    // Create a phong material
    duk::renderer::PhongMaterialDataSource phongMaterialDataSource = {};
    phongMaterialDataSource.albedo = glm::vec4(0.4f, 1.0f, 0.8f, 1.0f);
    phongMaterialDataSource.albedoTexture = imagePool->white_image();
    phongMaterialDataSource.specular = 32;
    phongMaterialDataSource.specularTexture = imagePool->white_image();
    phongMaterialDataSource.update_hash();
    cubeMeshRenderer->material = materialPool->create(duk::resource::Id(666), &phongMaterialDataSource);
    auto cubePosition = cubeObject.add<duk::renderer::Position3D>();
    cubePosition->value = glm::vec3(0,0,-10);
    auto cubeScale = cubeObject.add<duk::renderer::Scale3D>();
    cubeScale->value = glm::vec3(1.0f, 1.0f, 1.0f);
    auto cubeRotation = cubeObject.add<duk::renderer::Rotation3D>();
    cubeRotation->value = glm::radians(glm::vec3(30, 45, 0));

    //Show the window we created.
    window->show();

    //Setting the camera we created to be rendered in our render.
    renderer->use_as_camera(camera);

    //Creating a timer
    duk::tools::Timer timer;

    //Our main loop
    while (run) {

        timer.start();
        
        window->pool_events();

        //Rotating the cube in the X value by the timer total duration. 
        cubeRotation->value = glm::vec3(timer.total_duration().count(), 45.0f, 0.0f);

        //Telling to our renderer to render the scene we want.
        renderer->render(scene.get());

        timer.stop();
    }
    
    return 0;
}
