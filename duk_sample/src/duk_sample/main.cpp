//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/mesh/mesh_data_source.h>
#include <duk_renderer/pipeline/std_shader_data_source.h>
#include <duk_renderer/renderer.h>
#include <duk_macros/assert.h>

#include <duk_platform/window.h>
#include <duk_log/logger.h>
#include <duk_log/sink_std_console.h>

#include <fstream>

using namespace duk::renderer;
using namespace duk::platform;
using namespace duk::log;
using namespace duk::task;

namespace detail {

std::vector<uint8_t> load_spir_v(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to open spir-V file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    std::vector<uint8_t> buffer(file.tellg());
    file.seekg(0);
    file.read((char*)buffer.data(), buffer.size());

    return buffer;
}

StdShaderDataSource load_shader_data_source(const char* vertexFilepath, const char* fragmentFilepath) {
    StdShaderDataSource dataSource;

    dataSource.insert_spir_v_code(Shader::Module::VERTEX, load_spir_v(vertexFilepath));
    dataSource.insert_spir_v_code(Shader::Module::FRAGMENT, load_spir_v(fragmentFilepath));

    dataSource.update_hash();

    return dataSource;
}

}

int main() {

    Logger logger(Level::DEBUG);
    SinkStdConsole consoleSink;
    consoleSink.flush_from(logger);

    logger.log(Level::WARN) << "safe";

    logger.print_error("my value is {0}", 120);

    WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = "Duk";
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    auto expectedWindow = Window::create_window(windowCreateInfo);

    if (!expectedWindow) {
        logger.log(Level::ERR) << "Failed to create window!";
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
    rendererCreateInfo.logger = &logger;

    auto expectedRenderer = Renderer::create_renderer(rendererCreateInfo);

    if (!expectedRenderer) {
        logger.log(Level::ERR) << "Failed to create renderer: " << expectedRenderer.error().description();
        return 1;
    }
    auto renderer = std::move(expectedRenderer.value());

    duk::events::EventListener listener;

    bool run = true;

    listener.listen(window->window_close_event, [&window, &logger](auto){
        logger.print_verb("Window asked to be closed");
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run, &logger](auto){
        logger.print_verb("Window was destroyed");
        run = false;
    });

    auto outputImage = renderer->present_image();

    AttachmentDescription colorAttachmentDescription = {};
    colorAttachmentDescription.format = outputImage->format();
    colorAttachmentDescription.initialLayout = ImageLayout::UNDEFINED;
    colorAttachmentDescription.layout = ImageLayout::COLOR_ATTACHMENT;
    colorAttachmentDescription.finalLayout = ImageLayout::PRESENT_SRC;
    colorAttachmentDescription.storeOp = StoreOp::STORE;
    colorAttachmentDescription.loadOp = LoadOp::CLEAR;

    Renderer::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.colorAttachments = &colorAttachmentDescription;
    renderPassCreateInfo.colorAttachmentCount = 1;

    auto expectedRenderPass = renderer->create_render_pass(renderPassCreateInfo);

    if (!expectedRenderPass){
        logger.log(Level::ERR) << "failed to create render pass: " << expectedRenderPass.error().description();
        return 1;
    }

    auto renderPass = std::move(expectedRenderPass.value());

    auto triangleShaderDataSource = ::detail::load_shader_data_source("triangle.vert.spv", "triangle.frag.spv");

    Renderer::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = &triangleShaderDataSource;

    auto expectedShader = renderer->create_shader(shaderCreateInfo);

    if (!expectedShader) {
        logger.print_error("failed to create shader: {0}", expectedShader.error().description());
    }

    auto shader = std::move(expectedShader.value());

    Renderer::PipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport.extent = {window->width(), window->height()};
    pipelineCreateInfo.viewport.maxDepth = 1.0f;
    pipelineCreateInfo.viewport.minDepth = 0.0f;
    pipelineCreateInfo.scissor.extent = pipelineCreateInfo.viewport.extent;
    pipelineCreateInfo.shader = shader.get();
    pipelineCreateInfo.renderPass = renderPass.get();

    auto expectedPipeline = renderer->create_pipeline(pipelineCreateInfo);

    if (!expectedPipeline) {
        logger.print_error("failed to create pipeline: {0}", expectedPipeline.error().description());
    }

    auto pipeline = std::move(expectedPipeline.value());

    Renderer::FrameBufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.attachmentCount = 1;
    frameBufferCreateInfo.attachments = outputImage;
    frameBufferCreateInfo.renderPass = renderPass.get();
    frameBufferCreateInfo.width = window->width();
    frameBufferCreateInfo.height = window->height();

    auto expectedFrameBuffer = renderer->create_frame_buffer(frameBufferCreateInfo);

    if (!expectedFrameBuffer){
        logger.log(Level::ERR) << "failed to create frame buffer: " << expectedFrameBuffer.error().description();
        return 1;
    }

    auto frameBuffer = std::move(expectedFrameBuffer.value());

    Renderer::CommandQueueCreateInfo graphicsCommandQueueCreateInfo = {};
    graphicsCommandQueueCreateInfo.type = CommandQueueType::QUEUE_GRAPHICS;

    auto expectedQueue = renderer->create_command_queue(graphicsCommandQueueCreateInfo);

    if (!expectedQueue) {
        logger.log(Level::ERR) << "failed to create graphics command queue: " << expectedQueue.error().description();
        return 1;
    }

    auto graphicsQueue = std::move(expectedQueue.value());

    auto expectedScheduler = renderer->create_command_scheduler();

    if (!expectedScheduler) {
        logger.log(Level::ERR) << "failed to create command scheduler: " << expectedScheduler.error().description();
        return 1;
    }

    auto scheduler = std::move(expectedScheduler.value());

    while (run){
        window->pool_events();

        renderer->prepare_frame();

        scheduler->begin();

        auto acquireImageCommand = scheduler->schedule(renderer->acquire_image_command());

        auto mainRenderPassCommand = scheduler->schedule(graphicsQueue->enqueue([&renderPass, &frameBuffer, &pipeline](CommandBuffer* commandBuffer) {

            commandBuffer->begin();
            CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass = renderPass.get();
            renderPassBeginInfo.frameBuffer = frameBuffer.get();

            commandBuffer->begin_render_pass(renderPassBeginInfo);

            commandBuffer->bind_pipeline(pipeline.get());

            commandBuffer->draw(3, 0, 0, 0);

            commandBuffer->end_render_pass();

            commandBuffer->end();
        }));

        auto presentCommand = scheduler->schedule(renderer->present_command());

        acquireImageCommand.then(mainRenderPassCommand).then(presentCommand);

        scheduler->flush();
    }

    graphicsQueue.reset();

    logger.log(Level::INFO) << "Closed!";

    return 0;
}