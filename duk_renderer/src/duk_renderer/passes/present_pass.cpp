/// 05/10/2023
/// present_pass.cpp

#include <duk_platform/window.h>
#include <duk_renderer/passes/present_pass.h>
#include <duk_renderer/shader/shader_pipeline_builtins.h>

namespace duk::renderer {

namespace detail {

static glm::vec2 compute_scale_to_fit_aspect_ratio(const glm::vec2& inputSize, const glm::vec2& outputSize) {
    const auto inputAspectRatio = inputSize.x / inputSize.y;
    const auto outputAspectRatio = outputSize.x / outputSize.y;
    glm::vec2 scale = glm::vec2(1.0f);
    if (inputAspectRatio > outputAspectRatio) {
        scale = glm::vec2(1.0, outputAspectRatio / inputAspectRatio);
    } else if (inputAspectRatio < outputAspectRatio) {
        scale = glm::vec2(inputAspectRatio / outputAspectRatio, 1.0);
    }
    return scale;
}

}// namespace detail

PresentPass::PresentPass(const PresentPassCreateInfo& presentPassCreateInfo)
    : m_rhi(presentPassCreateInfo.rhi)
    , m_inColor(duk::rhi::Access::SHADER_READ, duk::rhi::PipelineStage::FRAGMENT_SHADER, duk::rhi::Image::Layout::SHADER_READ_ONLY) {
    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = m_rhi->present_image()->format();
        colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
        colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::PRESENT_SRC;
        colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
        colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription attachmentDescriptions[] = {colorAttachmentDescription};

        duk::rhi::RHI::RenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.colorAttachments = attachmentDescriptions;
        renderPassCreateInfo.colorAttachmentCount = std::size(attachmentDescriptions);

        m_renderPass = m_rhi->create_render_pass(renderPassCreateInfo);
    }

    {
        MaterialCreateInfo materialCreateInfo = {};
        materialCreateInfo.rhi = m_rhi;
        materialCreateInfo.commandQueue = presentPassCreateInfo.commandQueue;
        materialCreateInfo.materialData.shader = presentPassCreateInfo.shader;

        auto& properties = materialCreateInfo.materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::UNIFORM;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& scale = bindingData->members.emplace_back();
                scale.name = "scale";
                scale.type = BufferBinding::Member::Type::VEC2;
                scale.data.vec2Value = glm::vec2(1.0f);
            }
            return bindingData;
        }();

        m_fullscreenMaterial = std::make_unique<Material>(materialCreateInfo);
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

    const auto outputSize = glm::vec2(m_rhi->present_image()->size());

    if (!m_frameBuffer || outputSize != glm::vec2(m_frameBuffer->size())) {
        duk::rhi::Image* frameBufferAttachments[] = {m_rhi->present_image()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        m_frameBuffer = m_rhi->create_frame_buffer(frameBufferCreateInfo);
    }

    m_fullscreenMaterial->set("uTexture", duk::rhi::Descriptor::image_sampler(m_inColor.image(), m_inColor.image_layout(), {duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_BORDER}));

    const auto inputSize = glm::vec2(m_inColor.image()->size());
    const auto scale = detail::compute_scale_to_fit_aspect_ratio(inputSize, outputSize);

    m_fullscreenMaterial->set("uProperties", "scale", scale);

    m_fullscreenMaterial->update(*params.pipelineCache, m_renderPass.get(), outputSize);
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
