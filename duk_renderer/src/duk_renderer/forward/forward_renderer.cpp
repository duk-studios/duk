/// 01/08/2023
/// forward_renderer.cpp

#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/components/mesh_painter.h>

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <fstream>

namespace duk::renderer {

ForwardRenderer::ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo) :
    Renderer(forwardRendererCreateInfo.rendererCreateInfo) {

    {
        m_depthImage = create_depth_image(render_width(), render_height());
    }

    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = m_rhi->present_image()->format();
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

        auto expectedRenderPass = m_rhi->create_render_pass(renderPassCreateInfo);

        if (!expectedRenderPass) {
            throw std::runtime_error("failed to create RenderPass: " + expectedRenderPass.error().description());
        }

        CanvasCreateInfo canvasCreateInfo = {};
        canvasCreateInfo.renderPass = std::move(expectedRenderPass.value());
        canvasCreateInfo.width = render_width();
        canvasCreateInfo.height = render_height();

        m_canvas = std::make_unique<Canvas>(canvasCreateInfo);
    }

    {
        duk::rhi::Image* frameBufferAttachments[] = {m_rhi->present_image(), m_depthImage.get()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_canvas->render_pass();

        auto expectedFrameBuffer = m_rhi->create_frame_buffer(frameBufferCreateInfo);

        if (!expectedFrameBuffer) {
            throw std::runtime_error("failed to create FrameBuffer: " + expectedFrameBuffer.error().description());
        }

        m_frameBuffer = std::move(expectedFrameBuffer.value());
    }
}

void ForwardRenderer::render(duk::scene::Scene* scene) {

    m_rhi->prepare_frame();

    m_scheduler->begin();

    auto mainRenderPass = m_mainQueue->submit([this, scene](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::rhi::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();
        renderPassBeginInfo.renderPass = m_canvas->render_pass();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        for (auto object : scene->objects_with_components<MeshPainter>()) {
            auto meshPainter = object.component<MeshPainter>();

            Palette::UpdateParams updateParams = {};
            updateParams.object = object;

            meshPainter->palette->update(updateParams);

            Painter::PaintParams paintParams = {};
            paintParams.canvas = m_canvas.get();
            paintParams.mesh = meshPainter->mesh;
            paintParams.palette = meshPainter->palette;

            meshPainter->painter->paint(commandBuffer, paintParams);
        }

        commandBuffer->end_render_pass();

        commandBuffer->end();
    });

    auto acquireImageCommand = m_scheduler->schedule(m_rhi->acquire_image_command());
    auto mainRenderPassCommand = m_scheduler->schedule(std::move(mainRenderPass));
    auto presentCommand = m_scheduler->schedule(m_rhi->present_command());

    mainRenderPassCommand.wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT);

    presentCommand.wait(mainRenderPassCommand);

    m_scheduler->flush();
}

void ForwardRenderer::resize(uint32_t width, uint32_t height) {
    Renderer::resize(width, height);
    duk::rhi::EmptyImageDataSource depthImageDataSource(render_width(), render_height(), m_rhi->capabilities()->depth_format());
    depthImageDataSource.update_hash();
    m_depthImage->update(&depthImageDataSource);

    m_canvas->resize(width, height);
}

}
