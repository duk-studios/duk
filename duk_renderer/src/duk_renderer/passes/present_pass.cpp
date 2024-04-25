/// 05/10/2023
/// present_pass.cpp

#include <duk_platform/window.h>
#include <duk_renderer/material/fullscreen/fullscreen_material.h>
#include <duk_renderer/material/pipeline.h>
#include <duk_renderer/passes/present_pass.h>

namespace duk::renderer {

PresentPass::PresentPass(const PresentPassCreateInfo& presentPassCreateInfo)
    : m_renderer(presentPassCreateInfo.renderer)
    , m_inColor(duk::rhi::Access::SHADER_READ, duk::rhi::PipelineStage::FRAGMENT_SHADER, duk::rhi::Image::Layout::SHADER_READ_ONLY) {
    {
        //ensure that we recreate the framebuffer on this frame
        auto window = presentPassCreateInfo.window;
        m_listener.listen(window->window_resize_event, [this](uint32_t width, uint32_t height) {
            m_frameBuffer.reset();
        });
    }

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

        m_renderPass = m_renderer->rhi()->create_render_pass(renderPassCreateInfo);
    }

    {
        FullscreenMaterialDataSource fullscreenMaterialDataSource = {};
        fullscreenMaterialDataSource.sampler = duk::rhi::Sampler(duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_BORDER);
        fullscreenMaterialDataSource.update_hash();

        MaterialCreateInfo materialCreateInfo = {};
        materialCreateInfo.renderer = m_renderer;
        materialCreateInfo.materialDataSource = &fullscreenMaterialDataSource;

        m_fullscreenMaterial = std::make_unique<Material>(materialCreateInfo);

        m_fullscreenMaterialDescriptorSet = dynamic_cast<FullscreenMaterialDescriptorSet*>(m_fullscreenMaterial->descriptor_set());

        // necessary when using vulkan, need to take that into account when supporting other APIs
        m_fullscreenMaterial->pipeline()->invert_y(true);
    }
}

PresentPass::~PresentPass() = default;

PassConnection* PresentPass::in_color() {
    return &m_inColor;
}

void PresentPass::update(const Pass::UpdateParams& params) {
    if (!m_inColor.image()) {
        return;
    }

    if (!m_fullscreenMaterialDescriptorSet) {
        return;
    }

    if (!m_frameBuffer) {
        duk::rhi::Image* frameBufferAttachments[] = {m_renderer->rhi()->present_image()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        m_frameBuffer = m_renderer->rhi()->create_frame_buffer(frameBufferCreateInfo);
    }

    m_fullscreenMaterialDescriptorSet->set_image(m_inColor.image());

    DrawParams drawParams = {};
    drawParams.globalDescriptors = params.globalDescriptors;
    drawParams.outputWidth = params.outputWidth;
    drawParams.outputHeight = params.outputHeight;
    drawParams.renderPass = m_renderPass.get();

    m_fullscreenMaterial->update(drawParams);
}

void PresentPass::render(duk::rhi::CommandBuffer* commandBuffer) {
    // use a pipeline barrier to make sure that inColor is ready
    {
        auto imageMemoryBarrier = m_inColor.image_memory_barrier();

        duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarrierCount = 1;
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;

        commandBuffer->pipeline_barrier(pipelineBarrier);
    }

    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    m_fullscreenMaterial->bind(commandBuffer);

    // a single triangle that will cover the entire screen
    commandBuffer->draw(3, 1, 0, 0);

    commandBuffer->end_render_pass();
}

}// namespace duk::renderer
