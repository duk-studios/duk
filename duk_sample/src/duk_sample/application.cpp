/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_sample/color_shader_data_source.h>
#include <duk_sample/compute_shader_data_source.h>
#include <duk_platform/window.h>
#include <duk_renderer/pipeline/std_shader_data_source.h>
#include <duk_renderer/vertex_types.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <fstream>
#include <chrono>

namespace duk::sample {

struct Transform {
    glm::mat4 model;
};

namespace detail {

struct PixelRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

using PixelRGBAImageDataSource = duk::renderer::StdImageDataSource<PixelRGBA, duk::renderer::Image::PixelFormat::R8G8B8A8_UNORM>;

template<typename T>
T lerp(T from, T to, float percent) {
    return (from * (1 - percent)) + (to * percent);
}

PixelRGBAImageDataSource load_image() {
    PixelRGBAImageDataSource imageDataSource(64, 64);
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            auto xPercent = (float)x / 63;
            auto yPercent = (float)x / 63;

            auto r = lerp<uint8_t>(0, 255, xPercent);
            auto g = lerp<uint8_t>(0, 255, yPercent);
            auto b = lerp<uint8_t>(0, 255, xPercent * yPercent);

            imageDataSource.write_pixel(x, y, {r, g, b, 255});
        }
    }
    imageDataSource.update_hash();
    return imageDataSource;
}

}

struct UniformBuffer {
    glm::vec4 color;
};

std::vector<uint8_t> Application::load_bytes(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to open file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    std::vector<uint8_t> buffer(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

    return buffer;
}

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) :
    m_logger(duk::log::DEBUG),
    m_run(false) {

    if (applicationCreateInfo.sink) {
        applicationCreateInfo.sink->flush_from(m_logger);
    }

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = applicationCreateInfo.name;
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    m_window = check_expected_result(duk::platform::Window::create_window(windowCreateInfo));
    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this](){
        m_run = false;
    });

    m_listener.listen(m_window->window_resize_event, [this](uint32_t width, uint32_t height){
        if (m_window->minimized()) {
            return;
        }
        duk::renderer::EmptyImageDataSource depthImageDataSource(m_window->width(), m_window->height(), m_renderer->capabilities()->depth_format());
        depthImageDataSource.update_hash();
        m_depthImage->update(&depthImageDataSource);


        duk::renderer::GraphicsPipeline::Viewport viewport = {};
        viewport.extent = {width, height};
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        m_graphicsPipeline->set_viewport(viewport);

        duk::renderer::GraphicsPipeline::Scissor scissor = {};
        scissor.extent = viewport.extent;

        m_graphicsPipeline->set_scissor(scissor);

        m_graphicsPipeline->flush();
    });

    duk::renderer::RendererCreateInfo rendererCreateInfo = {};
    rendererCreateInfo.window = m_window.get();
    rendererCreateInfo.logger = &m_logger;
    rendererCreateInfo.deviceIndex = 0;
    rendererCreateInfo.applicationVersion = 1;
    rendererCreateInfo.applicationName = applicationCreateInfo.name;
    rendererCreateInfo.engineVersion = 1;
    rendererCreateInfo.engineName = "duk_sample";
    rendererCreateInfo.api = duk::renderer::RendererAPI::VULKAN;

    m_renderer = check_expected_result(duk::renderer::Renderer::create_renderer(rendererCreateInfo));

    m_scheduler = check_expected_result(m_renderer->create_command_scheduler());

    duk::renderer::Renderer::CommandQueueCreateInfo mainCommandQueueCreateInfo = {};
    mainCommandQueueCreateInfo.type = duk::renderer::CommandQueue::Type::GRAPHICS;

    m_mainCommandQueue = check_expected_result(m_renderer->create_command_queue(mainCommandQueueCreateInfo));

    duk::renderer::Renderer::CommandQueueCreateInfo computeCommandQueue = {};
    computeCommandQueue.type = duk::renderer::CommandQueue::Type::COMPUTE;

    m_computeQueue = check_expected_result(m_renderer->create_command_queue(computeCommandQueue));

    duk::renderer::EmptyImageDataSource depthImageDataSource(m_window->width(), m_window->height(), m_renderer->capabilities()->depth_format());
    depthImageDataSource.update_hash();

    duk::renderer::Renderer::ImageCreateInfo depthImageCreateInfo = {};
    depthImageCreateInfo.usage = duk::renderer::Image::Usage::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.initialLayout = duk::renderer::Image::Layout::DEPTH_ATTACHMENT;
    depthImageCreateInfo.updateFrequency = duk::renderer::Image::UpdateFrequency::DEVICE_DYNAMIC;
    depthImageCreateInfo.imageDataSource = &depthImageDataSource;
    depthImageCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_depthImage = check_expected_result(m_renderer->create_image(depthImageCreateInfo));

    auto outputImage = m_renderer->present_image();

    duk::renderer::AttachmentDescription colorAttachmentDescription = {};
    colorAttachmentDescription.format = outputImage->format();
    colorAttachmentDescription.initialLayout = duk::renderer::Image::Layout::UNDEFINED;
    colorAttachmentDescription.layout = duk::renderer::Image::Layout::COLOR_ATTACHMENT;
    colorAttachmentDescription.finalLayout = duk::renderer::Image::Layout::PRESENT_SRC;
    colorAttachmentDescription.storeOp = duk::renderer::StoreOp::STORE;
    colorAttachmentDescription.loadOp = duk::renderer::LoadOp::CLEAR;

    duk::renderer::AttachmentDescription depthAttachmentDescription = {};
    depthAttachmentDescription.format = m_depthImage->format();
    depthAttachmentDescription.initialLayout = duk::renderer::Image::Layout::UNDEFINED;
    depthAttachmentDescription.layout = duk::renderer::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
    depthAttachmentDescription.finalLayout = duk::renderer::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
    depthAttachmentDescription.storeOp = duk::renderer::StoreOp::DONT_CARE;
    depthAttachmentDescription.loadOp = duk::renderer::LoadOp::CLEAR;

    duk::renderer::AttachmentDescription attachmentDescriptions[] = {colorAttachmentDescription};

    duk::renderer::Renderer::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.colorAttachments = attachmentDescriptions;
    renderPassCreateInfo.colorAttachmentCount = std::size(attachmentDescriptions);
    renderPassCreateInfo.depthAttachment = &depthAttachmentDescription;

    m_renderPass = check_expected_result(m_renderer->create_render_pass(renderPassCreateInfo));

    duk::renderer::Image* frameBufferAttachments[] = {outputImage, m_depthImage.get()};

    duk::renderer::Renderer::FrameBufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
    frameBufferCreateInfo.attachments = frameBufferAttachments;
    frameBufferCreateInfo.renderPass = m_renderPass.get();

    m_frameBuffer = check_expected_result(m_renderer->create_frame_buffer(frameBufferCreateInfo));

    ColorShaderDataSource colorShaderDataSource;

    duk::renderer::Renderer::ShaderCreateInfo colorShaderCreateInfo = {};
    colorShaderCreateInfo.shaderDataSource = &colorShaderDataSource;

    m_colorShader = check_expected_result(m_renderer->create_shader(colorShaderCreateInfo));

    ComputeShaderDataSource computeShaderDataSource;

    duk::renderer::Renderer::ShaderCreateInfo computeShaderCreateInfo = {};
    computeShaderCreateInfo.shaderDataSource = &computeShaderDataSource;

    m_computeShader = check_expected_result(m_renderer->create_shader(computeShaderCreateInfo));

    duk::renderer::Renderer::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport.extent = {m_window->width(), m_window->height()};
    pipelineCreateInfo.viewport.maxDepth = 1.0f;
    pipelineCreateInfo.viewport.minDepth = 0.0f;
    pipelineCreateInfo.scissor.extent = pipelineCreateInfo.viewport.extent;
    pipelineCreateInfo.cullModeMask = duk::renderer::GraphicsPipeline::CullMode::BACK;
    pipelineCreateInfo.fillMode = duk::renderer::GraphicsPipeline::FillMode::FILL;
    pipelineCreateInfo.topology = duk::renderer::GraphicsPipeline::Topology::TRIANGLE_LIST;
    pipelineCreateInfo.shader = m_colorShader.get();
    pipelineCreateInfo.renderPass = m_renderPass.get();
    pipelineCreateInfo.depthTesting = true;

    m_graphicsPipeline = check_expected_result(m_renderer->create_graphics_pipeline(pipelineCreateInfo));

    duk::renderer::Renderer::ComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.shader = m_computeShader.get();

    m_computePipeline = check_expected_result(m_renderer->create_compute_pipeline(computePipelineCreateInfo));

    std::array<duk::renderer::Vertex2DColorUV, 4> vertices = {};
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};


    duk::renderer::Renderer::BufferCreateInfo vertexBufferCreateInfo = {};
    vertexBufferCreateInfo.type = duk::renderer::Buffer::Type::VERTEX;
    vertexBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::STATIC;
    vertexBufferCreateInfo.elementCount = vertices.size();
    vertexBufferCreateInfo.elementSize = sizeof(duk::renderer::Vertex2DColorUV);
    vertexBufferCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_vertexBuffer = check_expected_result(m_renderer->create_buffer(vertexBufferCreateInfo));
    m_vertexBuffer->write(vertices);
    m_vertexBuffer->flush();

    std::array<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

    duk::renderer::Renderer::BufferCreateInfo indexBufferCreateInfo = {};
    indexBufferCreateInfo.type = duk::renderer::Buffer::Type::INDEX_16;
    indexBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::STATIC;
    indexBufferCreateInfo.elementCount = indices.size();
    indexBufferCreateInfo.elementSize = sizeof(uint16_t);
    indexBufferCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_indexBuffer = check_expected_result(m_renderer->create_buffer(indexBufferCreateInfo));
    m_indexBuffer->write(indices);
    m_indexBuffer->flush();

    duk::renderer::Renderer::BufferCreateInfo transformUniformBufferCreateInfo = {};
    transformUniformBufferCreateInfo.type = duk::renderer::Buffer::Type::UNIFORM;
    transformUniformBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    transformUniformBufferCreateInfo.elementCount = 1;
    transformUniformBufferCreateInfo.elementSize = sizeof(Transform);
    transformUniformBufferCreateInfo.commandQueue = m_mainCommandQueue.get();


    m_transformUniformBuffer = check_expected_result(m_renderer->create_buffer(transformUniformBufferCreateInfo));

    duk::renderer::Renderer::BufferCreateInfo materialUniformBufferCreateInfo = {};
    materialUniformBufferCreateInfo.type = duk::renderer::Buffer::Type::UNIFORM;
    materialUniformBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    materialUniformBufferCreateInfo.elementCount = 1;
    materialUniformBufferCreateInfo.elementSize = sizeof(UniformBuffer);
    materialUniformBufferCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_materialUniformBuffer = check_expected_result(m_renderer->create_buffer(materialUniformBufferCreateInfo));
    UniformBuffer uniformBuffer = {};
    uniformBuffer.color = glm::vec4(1);
    m_materialUniformBuffer->write(uniformBuffer);
    m_materialUniformBuffer->flush();

    auto imageDataSource = detail::load_image();

    duk::renderer::Renderer::ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.imageDataSource = &imageDataSource;
    imageCreateInfo.usage = duk::renderer::Image::Usage::SAMPLED_STORAGE;
    imageCreateInfo.initialLayout = duk::renderer::Image::Layout::GENERAL;
    imageCreateInfo.updateFrequency = duk::renderer::Image::UpdateFrequency::DEVICE_DYNAMIC;
    imageCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_image = check_expected_result(m_renderer->create_image(imageCreateInfo));

    duk::renderer::Renderer::DescriptorSetCreateInfo computeDescriptorSet = {};
    computeDescriptorSet.description = computeShaderDataSource.descriptor_set_descriptions().at(0);

    m_computeDescriptorSet = check_expected_result(m_renderer->create_descriptor_set(computeDescriptorSet));
    m_computeDescriptorSet->set(0, duk::renderer::Descriptor::storage_image(m_image.get(), duk::renderer::Image::Layout::GENERAL));
    m_computeDescriptorSet->flush();

    duk::renderer::Renderer::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = colorShaderDataSource.descriptor_set_descriptions().at(0);

    m_descriptorSet = check_expected_result(m_renderer->create_descriptor_set(descriptorSetCreateInfo));

    m_descriptorSet->set(0, duk::renderer::Descriptor::uniform_buffer(m_transformUniformBuffer.get()));
    m_descriptorSet->set(1, duk::renderer::Descriptor::uniform_buffer(m_materialUniformBuffer.get()));
    duk::renderer::Sampler sampler = {};
    sampler.filter = duk::renderer::Sampler::Filter::NEAREST;
    sampler.wrapMode = duk::renderer::Sampler::WrapMode::REPEAT;
    m_descriptorSet->set(2, duk::renderer::Descriptor::image_sampler(m_image.get(), renderer::Image::Layout::SHADER_READ_ONLY, sampler));

    m_descriptorSet->flush();
}

Application::~Application() {

}

void Application::run() {

    using clock = std::chrono::high_resolution_clock;

    m_run = true;

    m_window->show();

    double time = 0;
    double deltaTime = 0;

    while (m_run) {
        auto start = clock::now();
        m_window->pool_events();

        update(time, deltaTime);

        if (!m_window->minimized()) {
            draw();
        }
        auto end = clock::now();
        deltaTime = std::chrono::duration<double>(end - start).count();
        time += deltaTime;
    }
}

void Application::update(double time, double deltaTime) {

    Transform transform = {};
    glm::vec3 position = glm::vec3(std::sin(time), 0, 0);
    transform.model = glm::translate(glm::mat4(1), position);
    m_transformUniformBuffer->write(transform);
    m_transformUniformBuffer->flush();
}

void Application::draw() {
    m_renderer->prepare_frame();

    m_scheduler->begin();

    auto acquireImageCommand = m_scheduler->schedule(m_renderer->acquire_image_command());
    auto computePass = m_scheduler->schedule(compute_pass());
    auto mainRenderPassCommand = m_scheduler->schedule(main_render_pass());
    auto reacquireComputeResources = m_scheduler->schedule(reacquire_compute_resources());
    auto presentCommand = m_scheduler->schedule(m_renderer->present_command());

    mainRenderPassCommand
        .wait(acquireImageCommand, duk::renderer::PipelineStage::COLOR_ATTACHMENT_OUTPUT)
        .wait(computePass, duk::renderer::PipelineStage::FRAGMENT_SHADER);

    reacquireComputeResources.wait(mainRenderPassCommand, duk::renderer::PipelineStage::COMPUTE_SHADER);
    presentCommand.wait(reacquireComputeResources);

    m_scheduler->flush();

}

duk::renderer::FutureCommand Application::compute_pass() {
    return m_computeQueue->submit([this](duk::renderer::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        commandBuffer->bind_compute_pipeline(m_computePipeline.get());

        commandBuffer->bind_descriptor_set(m_computeDescriptorSet.get(), 0);

        commandBuffer->dispatch(m_image->width(), m_image->height(), 1);

        duk::renderer::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.srcStageMask = duk::renderer::PipelineStage::COMPUTE_SHADER;
        imageMemoryBarrier.dstStageMask = duk::renderer::PipelineStage::TOP_OF_PIPE;
        imageMemoryBarrier.subresourceRange = duk::renderer::Image::kFullSubresourceRange;
        imageMemoryBarrier.oldLayout = duk::renderer::Image::Layout::GENERAL;
        imageMemoryBarrier.newLayout = duk::renderer::Image::Layout::SHADER_READ_ONLY;
        imageMemoryBarrier.srcAccessMask = duk::renderer::Access::SHADER_WRITE;
        imageMemoryBarrier.dstAccessMask = duk::renderer::Access::SHADER_READ;
        imageMemoryBarrier.srcCommandQueue = m_computeQueue.get();
        imageMemoryBarrier.dstCommandQueue = m_mainCommandQueue.get();
        imageMemoryBarrier.image = m_image.get();

        duk::renderer::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
        pipelineBarrier.imageMemoryBarrierCount = 1;

        commandBuffer->pipeline_barrier(pipelineBarrier);

        commandBuffer->end();
    });
}

duk::renderer::FutureCommand Application::main_render_pass() {
    return m_mainCommandQueue->submit([this](duk::renderer::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        {
            duk::renderer::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
            imageMemoryBarrier.srcStageMask = duk::renderer::PipelineStage::BOTTOM_OF_PIPE;
            imageMemoryBarrier.dstStageMask = duk::renderer::PipelineStage::FRAGMENT_SHADER;
            imageMemoryBarrier.subresourceRange = duk::renderer::Image::kFullSubresourceRange;
            imageMemoryBarrier.oldLayout = duk::renderer::Image::Layout::GENERAL;
            imageMemoryBarrier.newLayout = duk::renderer::Image::Layout::SHADER_READ_ONLY;
            imageMemoryBarrier.srcAccessMask = duk::renderer::Access::SHADER_WRITE;
            imageMemoryBarrier.dstAccessMask = duk::renderer::Access::SHADER_READ;
            imageMemoryBarrier.srcCommandQueue = m_computeQueue.get();
            imageMemoryBarrier.dstCommandQueue = m_mainCommandQueue.get();
            imageMemoryBarrier.image = m_image.get();

            duk::renderer::CommandBuffer::PipelineBarrier pipelineBarrier = {};
            pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
            pipelineBarrier.imageMemoryBarrierCount = 1;

            commandBuffer->pipeline_barrier(pipelineBarrier);
        }

        duk::renderer::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass = m_renderPass.get();
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        commandBuffer->bind_graphics_pipeline(m_graphicsPipeline.get());

        commandBuffer->bind_vertex_buffer(m_vertexBuffer.get());

        commandBuffer->bind_index_buffer(m_indexBuffer.get());

        commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);

        commandBuffer->draw_indexed(m_indexBuffer->element_count(), 1, 0, 0, 0);

        commandBuffer->end_render_pass();

        {
            duk::renderer::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
            imageMemoryBarrier.srcStageMask = duk::renderer::PipelineStage::FRAGMENT_SHADER;
            imageMemoryBarrier.dstStageMask = duk::renderer::PipelineStage::TOP_OF_PIPE;
            imageMemoryBarrier.subresourceRange = duk::renderer::Image::kFullSubresourceRange;
            imageMemoryBarrier.oldLayout = duk::renderer::Image::Layout::UNDEFINED;
            imageMemoryBarrier.newLayout = duk::renderer::Image::Layout::GENERAL;
            imageMemoryBarrier.srcAccessMask = duk::renderer::Access::SHADER_READ;
            imageMemoryBarrier.dstAccessMask = duk::renderer::Access::SHADER_WRITE;
            imageMemoryBarrier.srcCommandQueue = m_mainCommandQueue.get();
            imageMemoryBarrier.dstCommandQueue = m_computeQueue.get();
            imageMemoryBarrier.image = m_image.get();

            duk::renderer::CommandBuffer::PipelineBarrier pipelineBarrier = {};
            pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
            pipelineBarrier.imageMemoryBarrierCount = 1;

            commandBuffer->pipeline_barrier(pipelineBarrier);
        }

        commandBuffer->end();
    });
}

duk::renderer::FutureCommand Application::reacquire_compute_resources() {
    return m_computeQueue->submit([this](duk::renderer::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::renderer::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.srcStageMask = duk::renderer::PipelineStage::BOTTOM_OF_PIPE;
        imageMemoryBarrier.dstStageMask = duk::renderer::PipelineStage::COMPUTE_SHADER;
        imageMemoryBarrier.subresourceRange = duk::renderer::Image::kFullSubresourceRange;
        imageMemoryBarrier.oldLayout = duk::renderer::Image::Layout::SHADER_READ_ONLY;
        imageMemoryBarrier.newLayout = duk::renderer::Image::Layout::GENERAL;
        imageMemoryBarrier.srcAccessMask = duk::renderer::Access::SHADER_READ;
        imageMemoryBarrier.dstAccessMask = duk::renderer::Access::SHADER_WRITE;
        imageMemoryBarrier.srcCommandQueue = m_mainCommandQueue.get();
        imageMemoryBarrier.dstCommandQueue = m_computeQueue.get();
        imageMemoryBarrier.image = m_image.get();

        duk::renderer::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
        pipelineBarrier.imageMemoryBarrierCount = 1;

        commandBuffer->pipeline_barrier(pipelineBarrier);

        commandBuffer->end();
    });
}

}

