/// 01/08/2023
/// forward_renderer.cpp

#include <duk_renderer/forward/forward_renderer.h>

namespace duk::renderer {

ForwardRenderer::ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo) {

}

void ForwardRenderer::render(duk::scene::Scene* scene) {
    auto mainRenderPass = m_mainQueue->submit([this, scene](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::rhi::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();
        renderPassBeginInfo.renderPass = m_renderPass.get();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        // for (auto drawGroup : drawGroups) {
        //     bind drawGroup.vertexBuffer
        //     bind drawGroup.indexBuffer
        //     draw

        commandBuffer->end();
    });

    m_rhi->prepare_frame();

    m_scheduler->begin();

    auto acquireImageCommand = m_scheduler->schedule(m_rhi->acquire_image_command());
    auto mainRenderPassCommand = m_scheduler->schedule(std::move(mainRenderPass));
    auto presentCommand = m_scheduler->schedule(m_rhi->present_command());

    mainRenderPassCommand.wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT);

    presentCommand.wait(mainRenderPassCommand);

    m_scheduler->flush();
}

}
