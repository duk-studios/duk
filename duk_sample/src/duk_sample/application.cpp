/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_platform/window.h>
#include <duk_renderer/pipeline/std_shader_data_source.h>

#include <fstream>
#include "duk_renderer/mesh/vertex_types.h"

namespace duk::sample {

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

    duk::renderer::DescriptorSetDescription descriptorSetDescription;
    auto& uniformBinding = descriptorSetDescription.bindings.emplace_back();
    uniformBinding.type = duk::renderer::DescriptorType::UNIFORM_BUFFER;
    uniformBinding.moduleMask = duk::renderer::Shader::Module::FRAGMENT;

    dataSource.insert_descriptor_set_description(descriptorSetDescription);

    dataSource.update_hash();

    return dataSource;
}

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

    auto outputImage = m_renderer->present_image();

    duk::renderer::AttachmentDescription colorAttachmentDescription = {};
    colorAttachmentDescription.format = outputImage->format();
    colorAttachmentDescription.initialLayout = duk::renderer::ImageLayout::UNDEFINED;
    colorAttachmentDescription.layout = duk::renderer::ImageLayout::COLOR_ATTACHMENT;
    colorAttachmentDescription.finalLayout = duk::renderer::ImageLayout::PRESENT_SRC;
    colorAttachmentDescription.storeOp = duk::renderer::StoreOp::STORE;
    colorAttachmentDescription.loadOp = duk::renderer::LoadOp::CLEAR;

    duk::renderer::Renderer::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.colorAttachments = &colorAttachmentDescription;
    renderPassCreateInfo.colorAttachmentCount = 1;

    m_renderPass = check_expected_result(m_renderer->create_render_pass(renderPassCreateInfo));

    duk::renderer::Renderer::FrameBufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.attachmentCount = 1;
    frameBufferCreateInfo.attachments = outputImage;
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

    m_pipeline = check_expected_result(m_renderer->create_pipeline(pipelineCreateInfo));

    std::array<duk::renderer::Vertex2DColor, 4> vertices = {};
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}};
    vertices[1] = {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}};
    vertices[2] = {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}};
    vertices[3] = {{0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}};


    duk::renderer::Renderer::BufferCreateInfo vertexBufferCreateInfo = {};
    vertexBufferCreateInfo.type = duk::renderer::Buffer::Type::VERTEX;
    vertexBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::STATIC;
    vertexBufferCreateInfo.size = vertices.size();
    vertexBufferCreateInfo.elementSize = sizeof(duk::renderer::Vertex2DColor);

    m_vertexBuffer = check_expected_result(m_renderer->create_buffer(vertexBufferCreateInfo));
    m_vertexBuffer->write(vertices);
    m_vertexBuffer->flush();

    std::array<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

    duk::renderer::Renderer::BufferCreateInfo indexBufferCreateInfo = {};
    indexBufferCreateInfo.type = duk::renderer::Buffer::Type::INDEX_16;
    indexBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    indexBufferCreateInfo.size = indices.size();
    indexBufferCreateInfo.elementSize = sizeof(uint16_t);

    m_indexBuffer = check_expected_result(m_renderer->create_buffer(indexBufferCreateInfo));
    m_indexBuffer->write(indices);
    m_indexBuffer->flush();

    struct UniformBuffer {
        glm::vec4 color;
    };

    duk::renderer::Renderer::BufferCreateInfo uniformBufferCreateInfo = {};
    uniformBufferCreateInfo.type = duk::renderer::Buffer::Type::UNIFORM;
    uniformBufferCreateInfo.updateFrequency = duk::renderer::Buffer::UpdateFrequency::DYNAMIC;
    uniformBufferCreateInfo.size = 1;
    uniformBufferCreateInfo.elementSize = sizeof(UniformBuffer);

    m_uniformBuffer = check_expected_result(m_renderer->create_buffer(uniformBufferCreateInfo));
    m_uniformBuffer->write(UniformBuffer{glm::vec4(1.0f, 0.3f, 0.6f, 1.0f)});
    m_uniformBuffer->flush();


    duk::renderer::Renderer::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = colorShaderDataSource.descriptor_set_descriptions().at(0);

    m_descriptorSet = check_expected_result(m_renderer->create_descriptor_set(descriptorSetCreateInfo));

    m_descriptorSet->at(0) = m_uniformBuffer.get();
    m_descriptorSet->flush();
}

Application::~Application() {

}

void Application::run() {
    m_run = true;

    m_window->show();

    while (m_run) {
        m_window->pool_events();

        update();

        if (!m_window->minimized()) {
            draw();
        }
    }
}

void Application::update() {

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
    return m_mainCommandQueue->enqueue([this](duk::renderer::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::renderer::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass = m_renderPass.get();
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        commandBuffer->bind_pipeline(m_pipeline.get());

        commandBuffer->bind_vertex_buffer(m_vertexBuffer.get());

        commandBuffer->bind_index_buffer(m_indexBuffer.get());

        commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);

        commandBuffer->draw_indexed(m_indexBuffer->size(), 1, 0, 0, 0);

        commandBuffer->end_render_pass();

        commandBuffer->end();
    });
}

}

