/// 05/10/2023
/// present_pass.cpp

#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_material.h>
#include <duk_renderer/resources/materials/pipeline.h>

namespace duk::renderer {

PresentPass::PresentPass(const PresentPassCreateInfo& presentPassCreateInfo)
    : m_renderer(presentPassCreateInfo.renderer)
    , m_inColor(duk::rhi::Access::SHADER_READ, duk::rhi::PipelineStage::FRAGMENT_SHADER, duk::rhi::Image::Layout::SHADER_READ_ONLY) {
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

void PresentPass::render(const Pass::RenderParams& renderParams) {
    if (!m_inColor.image()) {
        return;
    }

    if (!m_fullscreenMaterialDescriptorSet) {
        return;
    }

    if (!m_frameBuffer || m_frameBuffer->width() != renderParams.outputWidth || m_frameBuffer->height() != renderParams.outputHeight) {
        duk::rhi::Image* frameBufferAttachments[] = {m_renderer->rhi()->present_image()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        m_frameBuffer = m_renderer->rhi()->create_frame_buffer(frameBufferCreateInfo);
    }

    m_fullscreenMaterialDescriptorSet->set_image(m_inColor.image());

    auto commandBuffer = renderParams.commandBuffer;

    // use a pipeline barrier to make sure that inColor is ready
    {
        auto imageMemoryBarrier = m_inColor.image_memory_barrier();

        duk::rhi::CommandBuffer::PipelineBarrier pipelineBarrier = {};
        pipelineBarrier.imageMemoryBarrierCount = 1;
        pipelineBarrier.imageMemoryBarriers = &imageMemoryBarrier;

        commandBuffer->pipeline_barrier(pipelineBarrier);
    }

    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    DrawParams drawParams = {};
    drawParams.globalDescriptors = renderParams.globalDescriptors;
    drawParams.outputWidth = renderParams.outputWidth;
    drawParams.outputHeight = renderParams.outputHeight;
    drawParams.renderPass = m_renderPass.get();

    m_fullscreenMaterial->bind(commandBuffer, drawParams);

    // a single triangle that will cover the entire screen
    commandBuffer->draw(3, 0, 1, 0);

    commandBuffer->end_render_pass();
}

PassConnection* PresentPass::in_color() {
    return &m_inColor;
}

}// namespace duk::renderer
