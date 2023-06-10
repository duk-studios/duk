/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
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

std::vector<uint8_t> load_bytes(const char* filepath) {
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

duk::renderer::StdShaderDataSource load_color_shader() {
    duk::renderer::StdShaderDataSource dataSource;

    dataSource.insert_spir_v_code(duk::renderer::Shader::Module::VERTEX, load_bytes("triangle.vert.spv"));
    dataSource.insert_spir_v_code(duk::renderer::Shader::Module::FRAGMENT, load_bytes("triangle.frag.spv"));

    dataSource.insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC2);
    dataSource.insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC4);
    dataSource.insert_vertex_attribute(duk::renderer::VertexAttribute::Format::VEC2);

    duk::renderer::DescriptorSetDescription descriptorSetDescription;
    auto& transformBinding = descriptorSetDescription.bindings.emplace_back();
    transformBinding.type = renderer::DescriptorType::UNIFORM_BUFFER;
    transformBinding.moduleMask = duk::renderer::Shader::Module::VERTEX;

    auto& uniformBinding = descriptorSetDescription.bindings.emplace_back();
    uniformBinding.type = duk::renderer::DescriptorType::UNIFORM_BUFFER;
    uniformBinding.moduleMask = duk::renderer::Shader::Module::FRAGMENT;

    auto& samplerBinding = descriptorSetDescription.bindings.emplace_back();
    samplerBinding.type = duk::renderer::DescriptorType::IMAGE_SAMPLER;
    samplerBinding.moduleMask = duk::renderer::Shader::Module::FRAGMENT;

    dataSource.insert_descriptor_set_description(descriptorSetDescription);

    dataSource.update_hash();

    return dataSource;
}

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


        duk::renderer::Pipeline::Viewport viewport = {};
        viewport.extent = {width, height};
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        m_pipeline->set_viewport(viewport);

        duk::renderer::Pipeline::Scissor scissor = {};
        scissor.extent = viewport.extent;

        m_pipeline->set_scissor(scissor);

        m_pipeline->flush();
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

    duk::renderer::Renderer::CommandQueueCreateInfo commandQueueCreateInfo = {};
    commandQueueCreateInfo.type = duk::renderer::CommandQueueType::QUEUE_GRAPHICS;

    m_mainCommandQueue = check_expected_result(m_renderer->create_command_queue(commandQueueCreateInfo));

    duk::renderer::EmptyImageDataSource depthImageDataSource(m_window->width(), m_window->height(), m_renderer->capabilities()->depth_format());
    depthImageDataSource.update_hash();

    duk::renderer::Renderer::ImageCreateInfo depthImageCreateInfo = {};
    depthImageCreateInfo.usage = duk::renderer::Image::Usage::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.initialLayout = duk::renderer::Image::Layout::DEPTH_ATTACHMENT;
    depthImageCreateInfo.updateFrequency = duk::renderer::Image::UpdateFrequency::DEVICE_DYNAMIC;
    depthImageCreateInfo.imageDataSource = &depthImageDataSource;

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

    auto colorShaderDataSource = detail::load_color_shader();

    duk::renderer::Renderer::ShaderCreateInfo colorShaderCreateInfo = {};
    colorShaderCreateInfo.shaderDataSource = &colorShaderDataSource;

    m_colorShader = check_expected_result(m_renderer->create_shader(colorShaderCreateInfo));

    duk::renderer::Renderer::PipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.viewport.extent = {m_window->width(), m_window->height()};
    pipelineCreateInfo.viewport.maxDepth = 1.0f;
    pipelineCreateInfo.viewport.minDepth = 0.0f;
    pipelineCreateInfo.scissor.extent = pipelineCreateInfo.viewport.extent;
    pipelineCreateInfo.cullModeMask = duk::renderer::Pipeline::CullMode::BACK;
    pipelineCreateInfo.shader = m_colorShader.get();
    pipelineCreateInfo.renderPass = m_renderPass.get();
    pipelineCreateInfo.depthTesting = true;

    m_pipeline = check_expected_result(m_renderer->create_pipeline(pipelineCreateInfo));

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

    m_vertexBuffer = check_expected_result(m_renderer->create_buffer(vertexBufferCreateInfo));
    m_vertexBuffer->write(vertices);
    m_vertexBuffer->flush();

    std::array<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

    duk::renderer::Renderer::BufferCreateInfo indexBufferCreateInfo = {};
    indexBufferCreateInfo.type = duk::renderer::Buffer::Type::INDEX_16;
    indexBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::STATIC;
    indexBufferCreateInfo.elementCount = indices.size();
    indexBufferCreateInfo.elementSize = sizeof(uint16_t);

    m_indexBuffer = check_expected_result(m_renderer->create_buffer(indexBufferCreateInfo));
    m_indexBuffer->write(indices);
    m_indexBuffer->flush();

    duk::renderer::Renderer::BufferCreateInfo transformUniformBufferCreateInfo = {};
    transformUniformBufferCreateInfo.type = duk::renderer::Buffer::Type::UNIFORM;
    transformUniformBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    transformUniformBufferCreateInfo.elementCount = 1;
    transformUniformBufferCreateInfo.elementSize = sizeof(Transform);

    m_transformUniformBuffer = check_expected_result(m_renderer->create_buffer(transformUniformBufferCreateInfo));

    duk::renderer::Renderer::BufferCreateInfo materialUniformBufferCreateInfo = {};
    materialUniformBufferCreateInfo.type = duk::renderer::Buffer::Type::UNIFORM;
    materialUniformBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    materialUniformBufferCreateInfo.elementCount = 1;
    materialUniformBufferCreateInfo.elementSize = sizeof(UniformBuffer);

    m_materialUniformBuffer = check_expected_result(m_renderer->create_buffer(materialUniformBufferCreateInfo));
    UniformBuffer uniformBuffer = {};
    uniformBuffer.color = glm::vec4(1);
    m_materialUniformBuffer->write(uniformBuffer);
    m_materialUniformBuffer->flush();

    auto imageDataSource = detail::load_image();

    duk::renderer::Renderer::ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.imageDataSource = &imageDataSource;
    imageCreateInfo.usage = duk::renderer::Image::Usage::SAMPLED;
    imageCreateInfo.initialLayout = duk::renderer::Image::Layout::SHADER_READ_ONLY;
    imageCreateInfo.updateFrequency = duk::renderer::Image::UpdateFrequency::STATIC;

    m_image = check_expected_result(m_renderer->create_image(imageCreateInfo));

    duk::renderer::Renderer::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = colorShaderDataSource.descriptor_set_descriptions().at(0);

    m_descriptorSet = check_expected_result(m_renderer->create_descriptor_set(descriptorSetCreateInfo));

    duk::renderer::Descriptor transformDescriptor(m_transformUniformBuffer.get());
    m_descriptorSet->set(0, transformDescriptor);

    duk::renderer::Descriptor materialDescriptor(m_materialUniformBuffer.get());
    m_descriptorSet->set(1, materialDescriptor);

    duk::renderer::Sampler sampler = {};
    sampler.filter = duk::renderer::Sampler::Filter::NEAREST;
    sampler.wrapMode = duk::renderer::Sampler::WrapMode::REPEAT;

    duk::renderer::Descriptor colorImageDescriptor(m_image.get(), duk::renderer::Image::Layout::SHADER_READ_ONLY, sampler);
    m_descriptorSet->set(2, colorImageDescriptor);

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
    auto mainRenderPassCommand = m_scheduler->schedule(main_render_pass());
    auto presentCommand = m_scheduler->schedule(m_renderer->present_command());

    acquireImageCommand.then(mainRenderPassCommand).then(presentCommand);

    m_scheduler->flush();

}

duk::renderer::FutureCommand Application::main_render_pass() {
    return m_mainCommandQueue->submit([this](duk::renderer::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::renderer::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass = m_renderPass.get();
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        commandBuffer->bind_pipeline(m_pipeline.get());

        commandBuffer->bind_vertex_buffer(m_vertexBuffer.get());

        commandBuffer->bind_index_buffer(m_indexBuffer.get());

        commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);

        commandBuffer->draw_indexed(m_indexBuffer->element_count(), 1, 0, 0, 0);

        commandBuffer->end_render_pass();

        commandBuffer->end();
    });
}

}

