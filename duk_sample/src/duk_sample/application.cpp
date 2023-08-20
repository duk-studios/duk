/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_sample/color_shader_data_source.h>
#include <duk_sample/compute_shader_data_source.h>
#include <duk_platform/window.h>
#include <duk_rhi/pipeline/std_shader_data_source.h>
#include <duk_rhi/vertex_types.h>

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

using PixelRGBAImageDataSource = duk::rhi::StdImageDataSource<PixelRGBA, duk::rhi::Image::PixelFormat::R8G8B8A8_UNORM>;

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

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::rhi::Vertex2DColorUV, uint16_t>;

DefaultMeshDataSource quad_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 4> vertices = {};
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 6> indices = {0, 1, 2, 2, 1, 3};

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
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
        duk::rhi::EmptyImageDataSource depthImageDataSource(m_window->width(), m_window->height(), m_rhi->capabilities()->depth_format());
        depthImageDataSource.update_hash();
        m_depthImage->update(&depthImageDataSource);


        duk::rhi::GraphicsPipeline::Viewport viewport = {};
        viewport.extent = {width, height};
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        m_graphicsPipeline->set_viewport(viewport);

        duk::rhi::GraphicsPipeline::Scissor scissor = {};
        scissor.extent = viewport.extent;

        m_graphicsPipeline->set_scissor(scissor);

        m_graphicsPipeline->flush();
    });

    duk::rhi::RHICreateInfo rendererCreateInfo = {};
    rendererCreateInfo.window = m_window.get();
    rendererCreateInfo.logger = &m_logger;
    rendererCreateInfo.deviceIndex = 0;
    rendererCreateInfo.applicationVersion = 1;
    rendererCreateInfo.applicationName = applicationCreateInfo.name;
    rendererCreateInfo.engineVersion = 1;
    rendererCreateInfo.engineName = "duk_sample";
    rendererCreateInfo.api = duk::rhi::API::VULKAN;

    m_rhi = check_expected_result(duk::rhi::RHI::create_rhi(rendererCreateInfo));

    m_scheduler = check_expected_result(m_rhi->create_command_scheduler());

    duk::rhi::RHI::CommandQueueCreateInfo mainCommandQueueCreateInfo = {};
    mainCommandQueueCreateInfo.type = duk::rhi::CommandQueue::Type::GRAPHICS;

    m_mainCommandQueue = check_expected_result(m_rhi->create_command_queue(mainCommandQueueCreateInfo));

    duk::rhi::RHI::CommandQueueCreateInfo computeCommandQueue = {};
    computeCommandQueue.type = duk::rhi::CommandQueue::Type::COMPUTE;

    m_computeQueue = check_expected_result(m_rhi->create_command_queue(computeCommandQueue));

    duk::rhi::EmptyImageDataSource depthImageDataSource(m_window->width(), m_window->height(), m_rhi->capabilities()->depth_format());
    depthImageDataSource.update_hash();

    duk::rhi::RHI::ImageCreateInfo depthImageCreateInfo = {};
    depthImageCreateInfo.usage = duk::rhi::Image::Usage::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.initialLayout = duk::rhi::Image::Layout::DEPTH_ATTACHMENT;
    depthImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    depthImageCreateInfo.imageDataSource = &depthImageDataSource;
    depthImageCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_depthImage = check_expected_result(m_rhi->create_image(depthImageCreateInfo));

    auto outputImage = m_rhi->present_image();

    duk::rhi::AttachmentDescription colorAttachmentDescription = {};
    colorAttachmentDescription.format = outputImage->format();
    colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
    colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
    colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::PRESENT_SRC;
    colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
    colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

    duk::rhi::AttachmentDescription depthAttachmentDescription = {};
    depthAttachmentDescription.format = m_depthImage->format();
    depthAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
    depthAttachmentDescription.layout = duk::rhi::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
    depthAttachmentDescription.finalLayout = duk::rhi::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
    depthAttachmentDescription.storeOp = duk::rhi::StoreOp::DONT_CARE;
    depthAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

    duk::rhi::AttachmentDescription attachmentDescriptions[] = {colorAttachmentDescription};

    duk::rhi::RHI::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.colorAttachments = attachmentDescriptions;
    renderPassCreateInfo.colorAttachmentCount = std::size(attachmentDescriptions);
    renderPassCreateInfo.depthAttachment = &depthAttachmentDescription;

    m_renderPass = check_expected_result(m_rhi->create_render_pass(renderPassCreateInfo));

    duk::rhi::Image* frameBufferAttachments[] = {outputImage, m_depthImage.get()};

    duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
    frameBufferCreateInfo.attachments = frameBufferAttachments;
    frameBufferCreateInfo.renderPass = m_renderPass.get();

    m_frameBuffer = check_expected_result(m_rhi->create_frame_buffer(frameBufferCreateInfo));

    ColorShaderDataSource colorShaderDataSource;

    duk::rhi::RHI::ShaderCreateInfo colorShaderCreateInfo = {};
    colorShaderCreateInfo.shaderDataSource = &colorShaderDataSource;

    m_colorShader = check_expected_result(m_rhi->create_shader(colorShaderCreateInfo));

    ComputeShaderDataSource computeShaderDataSource;

    duk::rhi::RHI::ShaderCreateInfo computeShaderCreateInfo = {};
    computeShaderCreateInfo.shaderDataSource = &computeShaderDataSource;

    m_computeShader = check_expected_result(m_rhi->create_shader(computeShaderCreateInfo));

    duk::rhi::RHI::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport.extent = {m_window->width(), m_window->height()};
    pipelineCreateInfo.viewport.maxDepth = 1.0f;
    pipelineCreateInfo.viewport.minDepth = 0.0f;
    pipelineCreateInfo.scissor.extent = pipelineCreateInfo.viewport.extent;
    pipelineCreateInfo.cullModeMask = duk::rhi::GraphicsPipeline::CullMode::BACK;
    pipelineCreateInfo.fillMode = duk::rhi::GraphicsPipeline::FillMode::FILL;
    pipelineCreateInfo.topology = duk::rhi::GraphicsPipeline::Topology::TRIANGLE_LIST;
    pipelineCreateInfo.shader = m_colorShader.get();
    pipelineCreateInfo.renderPass = m_renderPass.get();
    pipelineCreateInfo.depthTesting = true;

    m_graphicsPipeline = check_expected_result(m_rhi->create_graphics_pipeline(pipelineCreateInfo));

    duk::rhi::RHI::ComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.shader = m_computeShader.get();

    m_computePipeline = check_expected_result(m_rhi->create_compute_pipeline(computePipelineCreateInfo));

    duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {};
    meshBufferPoolCreateInfo.rhi = m_rhi.get();
    meshBufferPoolCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_meshBufferPool = std::make_shared<renderer::MeshBufferPool>(meshBufferPoolCreateInfo);

    auto quadDataSource = detail::quad_mesh_data_source();

    m_mesh = m_meshBufferPool->create_mesh(&quadDataSource);

    duk::rhi::RHI::BufferCreateInfo transformUniformBufferCreateInfo = {};
    transformUniformBufferCreateInfo.type = duk::rhi::Buffer::Type::UNIFORM;
    transformUniformBufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    transformUniformBufferCreateInfo.elementCount = 1;
    transformUniformBufferCreateInfo.elementSize = sizeof(Transform);
    transformUniformBufferCreateInfo.commandQueue = m_mainCommandQueue.get();


    m_transformUniformBuffer = check_expected_result(m_rhi->create_buffer(transformUniformBufferCreateInfo));

    duk::rhi::RHI::BufferCreateInfo materialUniformBufferCreateInfo = {};
    materialUniformBufferCreateInfo.type = duk::rhi::Buffer::Type::UNIFORM;
    materialUniformBufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    materialUniformBufferCreateInfo.elementCount = 1;
    materialUniformBufferCreateInfo.elementSize = sizeof(UniformBuffer);
    materialUniformBufferCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_materialUniformBuffer = check_expected_result(m_rhi->create_buffer(materialUniformBufferCreateInfo));
    UniformBuffer uniformBuffer = {};
    uniformBuffer.color = glm::vec4(1);
    m_materialUniformBuffer->write(uniformBuffer);
    m_materialUniformBuffer->flush();

    auto imageDataSource = detail::load_image();

    duk::rhi::RHI::ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.imageDataSource = &imageDataSource;
    imageCreateInfo.usage = duk::rhi::Image::Usage::SAMPLED_STORAGE;
    imageCreateInfo.initialLayout = duk::rhi::Image::Layout::GENERAL;
    imageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    imageCreateInfo.commandQueue = m_mainCommandQueue.get();

    m_image = check_expected_result(m_rhi->create_image(imageCreateInfo));

    duk::rhi::RHI::DescriptorSetCreateInfo computeDescriptorSet = {};
    computeDescriptorSet.description = computeShaderDataSource.descriptor_set_descriptions().at(0);

    m_computeDescriptorSet = check_expected_result(m_rhi->create_descriptor_set(computeDescriptorSet));
    m_computeDescriptorSet->set(0, duk::rhi::Descriptor::storage_image(m_image.get(), duk::rhi::Image::Layout::GENERAL));
    m_computeDescriptorSet->flush();

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = colorShaderDataSource.descriptor_set_descriptions().at(0);

    m_descriptorSet = check_expected_result(m_rhi->create_descriptor_set(descriptorSetCreateInfo));

    m_descriptorSet->set(0, duk::rhi::Descriptor::uniform_buffer(m_transformUniformBuffer.get()));
    m_descriptorSet->set(1, duk::rhi::Descriptor::uniform_buffer(m_materialUniformBuffer.get()));
    duk::rhi::Sampler sampler = {};
    sampler.filter = duk::rhi::Sampler::Filter::NEAREST;
    sampler.wrapMode = duk::rhi::Sampler::WrapMode::REPEAT;
    m_descriptorSet->set(2, duk::rhi::Descriptor::image_sampler(m_image.get(), rhi::Image::Layout::SHADER_READ_ONLY, sampler));

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
    m_rhi->prepare_frame();

    m_scheduler->begin();

    auto acquireImageCommand = m_scheduler->schedule(m_rhi->acquire_image_command());
    auto computePass = m_scheduler->schedule(compute_pass());
    auto mainRenderPassCommand = m_scheduler->schedule(main_render_pass());
    auto reacquireComputeResources = m_scheduler->schedule(reacquire_compute_resources());
    auto presentCommand = m_scheduler->schedule(m_rhi->present_command());

    mainRenderPassCommand
        .wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT)
        .wait(computePass, duk::rhi::PipelineStage::FRAGMENT_SHADER);

    reacquireComputeResources.wait(mainRenderPassCommand, duk::rhi::PipelineStage::COMPUTE_SHADER);
    presentCommand.wait(reacquireComputeResources);

    m_scheduler->flush();

}

duk::rhi::FutureCommand Application::compute_pass() {
    return m_computeQueue->submit([this](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        commandBuffer->bind_compute_pipeline(m_computePipeline.get());

        commandBuffer->bind_descriptor_set(m_computeDescriptorSet.get(), 0);

        commandBuffer->dispatch(m_image->width(), m_image->height(), 1);

        duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.srcStageMask = duk::rhi::PipelineStage::COMPUTE_SHADER;
        imageMemoryBarrier.dstStageMask = duk::rhi::PipelineStage::TOP_OF_PIPE;
        imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;
        imageMemoryBarrier.oldLayout = duk::rhi::Image::Layout::GENERAL;
        imageMemoryBarrier.newLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
        imageMemoryBarrier.srcAccessMask = duk::rhi::Access::SHADER_WRITE;
        imageMemoryBarrier.dstAccessMask = duk::rhi::Access::SHADER_READ;
        imageMemoryBarrier.srcCommandQueue = m_computeQueue.get();
        imageMemoryBarrier.dstCommandQueue = m_mainCommandQueue.get();
        imageMemoryBarrier.image = m_image.get();

        duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
        pipelineBarrier.imageMemoryBarrierCount = 1;

        commandBuffer->pipeline_barrier(pipelineBarrier);

        commandBuffer->end();
    });
}

duk::rhi::FutureCommand Application::main_render_pass() {
    return m_mainCommandQueue->submit([this](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        {
            duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
            imageMemoryBarrier.srcStageMask = duk::rhi::PipelineStage::BOTTOM_OF_PIPE;
            imageMemoryBarrier.dstStageMask = duk::rhi::PipelineStage::FRAGMENT_SHADER;
            imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;
            imageMemoryBarrier.oldLayout = duk::rhi::Image::Layout::GENERAL;
            imageMemoryBarrier.newLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
            imageMemoryBarrier.srcAccessMask = duk::rhi::Access::SHADER_WRITE;
            imageMemoryBarrier.dstAccessMask = duk::rhi::Access::SHADER_READ;
            imageMemoryBarrier.srcCommandQueue = m_computeQueue.get();
            imageMemoryBarrier.dstCommandQueue = m_mainCommandQueue.get();
            imageMemoryBarrier.image = m_image.get();

            duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
            pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
            pipelineBarrier.imageMemoryBarrierCount = 1;

            commandBuffer->pipeline_barrier(pipelineBarrier);
        }

        duk::rhi::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass = m_renderPass.get();
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        commandBuffer->bind_graphics_pipeline(m_graphicsPipeline.get());

        commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);

        m_mesh->draw(commandBuffer);

        commandBuffer->end_render_pass();

        {
            duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
            imageMemoryBarrier.srcStageMask = duk::rhi::PipelineStage::FRAGMENT_SHADER;
            imageMemoryBarrier.dstStageMask = duk::rhi::PipelineStage::TOP_OF_PIPE;
            imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;
            imageMemoryBarrier.oldLayout = duk::rhi::Image::Layout::UNDEFINED;
            imageMemoryBarrier.newLayout = duk::rhi::Image::Layout::GENERAL;
            imageMemoryBarrier.srcAccessMask = duk::rhi::Access::SHADER_READ;
            imageMemoryBarrier.dstAccessMask = duk::rhi::Access::SHADER_WRITE;
            imageMemoryBarrier.srcCommandQueue = m_mainCommandQueue.get();
            imageMemoryBarrier.dstCommandQueue = m_computeQueue.get();
            imageMemoryBarrier.image = m_image.get();

            duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
            pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
            pipelineBarrier.imageMemoryBarrierCount = 1;

            commandBuffer->pipeline_barrier(pipelineBarrier);
        }

        commandBuffer->end();
    });
}

duk::rhi::FutureCommand Application::reacquire_compute_resources() {
    return m_computeQueue->submit([this](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.srcStageMask = duk::rhi::PipelineStage::BOTTOM_OF_PIPE;
        imageMemoryBarrier.dstStageMask = duk::rhi::PipelineStage::COMPUTE_SHADER;
        imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;
        imageMemoryBarrier.oldLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
        imageMemoryBarrier.newLayout = duk::rhi::Image::Layout::GENERAL;
        imageMemoryBarrier.srcAccessMask = duk::rhi::Access::SHADER_READ;
        imageMemoryBarrier.dstAccessMask = duk::rhi::Access::SHADER_WRITE;
        imageMemoryBarrier.srcCommandQueue = m_mainCommandQueue.get();
        imageMemoryBarrier.dstCommandQueue = m_computeQueue.get();
        imageMemoryBarrier.image = m_image.get();

        duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;
        pipelineBarrier.imageMemoryBarrierCount = 1;

        commandBuffer->pipeline_barrier(pipelineBarrier);

        commandBuffer->end();
    });
}

}

