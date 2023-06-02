//
// Created by Ricardo on 09/04/2023.
//

#include <duk_renderer/mesh/mesh_data_source.h>
#include <duk_renderer/pipeline/std_shader_data_source.h>
#include <duk_renderer/renderer.h>
#include <cassert>

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

    dataSource.insert_vertex_attribute(VertexAttribute::Format::VEC2);
    dataSource.insert_vertex_attribute(VertexAttribute::Format::VEC4);

    DescriptorSetDescription descriptorSetDescription;
    auto& uniformBinding = descriptorSetDescription.bindings.emplace_back();
    uniformBinding.type = DescriptorType::UNIFORM_BUFFER;
    uniformBinding.moduleMask = Shader::Module::FRAGMENT;

    dataSource.insert_descriptor_set_description(descriptorSetDescription);

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
    pipelineCreateInfo.cullModeMask = Pipeline::CullMode::BACK;
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

    std::array<Vertex2DColor, 4> vertices = {};
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}};
    vertices[1] = {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}};
    vertices[2] = {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}};
    vertices[3] = {{0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}};


    Renderer::BufferCreateInfo vertexBufferCreateInfo = {};
    vertexBufferCreateInfo.type = Buffer::Type::VERTEX;
    vertexBufferCreateInfo.updateFrequency = Buffer::UpdateFrequency::DYNAMIC;
    vertexBufferCreateInfo.size = vertices.size();
    vertexBufferCreateInfo.elementSize = sizeof(Vertex2DColor);

    auto expectedVertexBuffer = renderer->create_buffer(vertexBufferCreateInfo);

    if (!expectedVertexBuffer) {
        logger.log(Level::ERR) << "failed to create vertex buffer: " << expectedVertexBuffer.error().description();
        return 1;
    }

    auto vertexBuffer = std::move(expectedVertexBuffer.value());

    vertexBuffer->write(vertices);

    vertexBuffer->flush();

    std::array<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

    Renderer::BufferCreateInfo indexBufferCreateInfo = {};
    indexBufferCreateInfo.type = Buffer::Type::INDEX_16;
    indexBufferCreateInfo.updateFrequency = Buffer::UpdateFrequency::DYNAMIC;
    indexBufferCreateInfo.size = indices.size();
    indexBufferCreateInfo.elementSize = sizeof(uint16_t);

    auto expectedIndexBuffer = renderer->create_buffer(indexBufferCreateInfo);

    if (!expectedIndexBuffer) {
        logger.log(Level::ERR) << "failed to create index buffer: " << expectedIndexBuffer.error().description();
        return 1;
    }

    auto indexBuffer = std::move(expectedIndexBuffer.value());

    indexBuffer->write(indices);
    indexBuffer->flush();

    struct UniformBuffer {
        glm::vec4 color;
    };

    Renderer::BufferCreateInfo uniformBufferCreateInfo = {};
    uniformBufferCreateInfo.type = Buffer::Type::UNIFORM;
    uniformBufferCreateInfo.updateFrequency = Buffer::UpdateFrequency::DYNAMIC;
    uniformBufferCreateInfo.size = 1;
    uniformBufferCreateInfo.elementSize = sizeof(UniformBuffer);

    auto expectedUniformBuffer = renderer->create_buffer(uniformBufferCreateInfo);

    if (!expectedUniformBuffer) {
        logger.log(Level::ERR) << "failed to create uniform buffer:" << expectedUniformBuffer.error().description();
        return 1;
    }

    auto uniformBuffer = std::move(expectedUniformBuffer.value());

    uniformBuffer->write(UniformBuffer{glm::vec4(1.0f, 0.3f, 0.6f, 1.0f)});
    uniformBuffer->flush();

    Renderer::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = triangleShaderDataSource.descriptor_set_descriptions().at(0);

    auto expectedDescriptorSet = renderer->create_descriptor_set(descriptorSetCreateInfo);

    if (!expectedDescriptorSet) {
        logger.log(Level::ERR) << "failed to create descriptor set: " << expectedDescriptorSet.error().description();
    }

    auto descriptorSet = std::move(expectedDescriptorSet.value());

    descriptorSet->at(0) = uniformBuffer.get();
    descriptorSet->flush();


    while (run) {
        while (window->minimized()) {
            logger.print_debug("window minimized");
            window->wait_events();
        }

        window->pool_events();

        renderer->prepare_frame();

        scheduler->begin();

        auto acquireImageCommand = scheduler->schedule(renderer->acquire_image_command());

        auto mainRenderPassCommand = scheduler->schedule(graphicsQueue->enqueue([&renderPass, &frameBuffer, &pipeline, &vertexBuffer, &indexBuffer, &descriptorSet](CommandBuffer* commandBuffer) {

            commandBuffer->begin();
            CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass = renderPass.get();
            renderPassBeginInfo.frameBuffer = frameBuffer.get();

            commandBuffer->begin_render_pass(renderPassBeginInfo);

            commandBuffer->bind_pipeline(pipeline.get());

            commandBuffer->bind_vertex_buffer(vertexBuffer.get());

            commandBuffer->bind_index_buffer(indexBuffer.get());

            commandBuffer->bind_descriptor_set(descriptorSet.get(), 0);

            commandBuffer->draw_indexed(indexBuffer->size(), 1, 0, 0, 0);

            commandBuffer->end_render_pass();

            commandBuffer->end();
        }));

        auto presentCommand = scheduler->schedule(renderer->present_command());

        acquireImageCommand.then(mainRenderPassCommand).then(presentCommand);

        scheduler->flush();
    }

    logger.log(Level::INFO) << "Closing";

    return 0;
}