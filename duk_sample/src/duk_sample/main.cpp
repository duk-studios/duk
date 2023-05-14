//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/mesh/mesh_data_source.h>
#include <duk_renderer/renderer.h>

#include <duk_platform/window.h>
#include <duk_log/logger.h>
#include <duk_log/sink_std_console.h>

int main() {
    using namespace duk::renderer;
    using namespace duk::platform;
    using namespace duk::log;


    Logger logger;
    SinkStdConsole consoleSink;
    consoleSink.flush_from(logger);


    WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = "Duk";
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    auto expectedWindow = Window::create_window(windowCreateInfo);

    if (!expectedWindow) {
        logger.log() << "Failed to create window!";
        return 1;
    }

    auto window = expectedWindow.value();

    VertexDataSourceInterleaved<Vertex2DColor> vertexDataSource;

    vertexDataSource.vector() = {
            {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f, -0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    };

    auto layout = vertexDataSource.vertex_layout();

    MeshDataSource meshDataSource;

    meshDataSource.set_vertex_data_source(&vertexDataSource);

    RendererCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.api = RendererAPI::VULKAN;
    rendererCreateInfo.window = window.get();
    rendererCreateInfo.engineName = "Duk";
    rendererCreateInfo.engineVersion = 1;
    rendererCreateInfo.applicationName = "Quacker";
    rendererCreateInfo.applicationVersion = 1;
    rendererCreateInfo.deviceIndex = 0;

    auto expectedRenderer = Renderer::create_renderer(rendererCreateInfo);

    if (expectedRenderer) {
        logger.log() << "Renderer created!";
    }
    else {
        logger.log() << "Renderer failed to be created!";
        return 1;
    }

    duk::events::EventListener listener;

    bool run = true;

    listener.listen(window->window_close_event, [&window, &logger](auto){
        logger.log() << "Window asked to be closed";
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run, &logger](auto){
        logger.log() << "Window was destroyed";
        run = false;
    });

    while (run){
        window->pool_events();
    }

    logger.log() << "Closed!";

    return 0;
}