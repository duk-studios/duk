/// 05/10/2023
/// present_pass.cpp

#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/painters/fullscreen/fullscreen_painter.h>
#include <duk_renderer/painters/fullscreen/fullscreen_palette.h>

namespace duk::renderer {

namespace detail {

static void draw_fullscreen_triangle(duk::rhi::CommandBuffer* commandBuffer, size_t, size_t) {
    commandBuffer->draw(3, 0, 1, 0);
}

}

PresentPass::PresentPass(const PresentPassCreateInfo& presentPassCreateInfo) :
    m_renderer(presentPassCreateInfo.renderer),
    m_fullscreenTriangleBrush(detail::draw_fullscreen_triangle) {

    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = m_renderer->rhi()->present_image()->format();
        colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
        colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::PRESENT_SRC;
        colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
        colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription attachmentDescriptions[] = {colorAttachmentDescription};

        duk::rhi::RHI::RenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.colorAttachments = attachmentDescriptions;
        renderPassCreateInfo.colorAttachmentCount = std::size(attachmentDescriptions);

        auto expectedRenderPass = m_renderer->rhi()->create_render_pass(renderPassCreateInfo);

        if (!expectedRenderPass) {
            throw std::runtime_error("failed to create RenderPass: " + expectedRenderPass.error().description());
        }

        m_renderPass = std::move(expectedRenderPass.value());
    }

    {
        FullscreenPainterCreateInfo fullscreenPainterCreateInfo = {};
        fullscreenPainterCreateInfo.rhi = m_renderer->rhi();
        fullscreenPainterCreateInfo.commandQueue = m_renderer->main_command_queue();

        m_fullscreenPainter = std::make_unique<FullscreenPainter>(fullscreenPainterCreateInfo);
    }

    {
        m_fullscreenPalette = m_fullscreenPainter->make_palette();
    }
}

PresentPass::~PresentPass() = default;

void PresentPass::render(const Pass::RenderParams& renderParams) {

    if (!m_frameBuffer || m_frameBuffer->width() != renderParams.outputWidth || m_frameBuffer->height() != renderParams.outputHeight) {
        duk::rhi::Image* frameBufferAttachments[] = {m_renderer->rhi()->present_image()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        auto expectedFrameBuffer = m_renderer->rhi()->create_frame_buffer(frameBufferCreateInfo);

        if (!expectedFrameBuffer) {
            throw std::runtime_error("failed to create FrameBuffer: " + expectedFrameBuffer.error().description());
        }

        m_frameBuffer = std::move(expectedFrameBuffer.value());
    }

    m_fullscreenPalette->update(m_inColor.image(), duk::rhi::Sampler{duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_BORDER});

    auto commandBuffer = renderParams.commandBuffer;

    // use a pipeline barrier to make sure that inColor is ready
    {
        duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.image = m_inColor.image();
        imageMemoryBarrier.srcStageMask = duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT;
        imageMemoryBarrier.dstStageMask = duk::rhi::PipelineStage::FRAGMENT_SHADER;
        imageMemoryBarrier.srcAccessMask = duk::rhi::Access::COLOR_ATTACHMENT_WRITE;
        imageMemoryBarrier.dstAccessMask = duk::rhi::Access::SHADER_READ;
        imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;

        duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarrierCount = 1;
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;

        commandBuffer->pipeline_barrier(pipelineBarrier);
    }

    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    Painter::PaintParams paintParams = {};
    paintParams.outputWidth = renderParams.outputWidth;
    paintParams.outputHeight = renderParams.outputHeight;
    paintParams.renderPass = m_renderPass.get();
    paintParams.palette = m_fullscreenPalette.get();
    paintParams.brush = &m_fullscreenTriangleBrush;

    m_fullscreenPainter->paint(commandBuffer, paintParams);

    commandBuffer->end_render_pass();
}

PassConnection* PresentPass::in_color() {
    return &m_inColor;
}

}
