/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/components/mesh_drawing.h>

namespace duk::renderer {

static constexpr auto kColorFormat = duk::rhi::Image::PixelFormat::R8G8B8A8_UNORM;

ForwardPass::ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo) :
    m_renderer(forwardPassCreateInfo.renderer),
    m_outColor(duk::rhi::Access::COLOR_ATTACHMENT_WRITE, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT) {

    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = kColorFormat;
        colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
        colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
        colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
        colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription depthAttachmentDescription = {};
        depthAttachmentDescription.format = m_renderer->rhi()->capabilities()->depth_format();
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

        auto expectedRenderPass = m_renderer->rhi()->create_render_pass(renderPassCreateInfo);

        if (!expectedRenderPass) {
            throw std::runtime_error("failed to create RenderPass: " + expectedRenderPass.error().description());
        }

        m_renderPass = std::move(expectedRenderPass.value());
    }
}

ForwardPass::~ForwardPass() = default;

void ForwardPass::render(const RenderParams& renderParams) {

    if (!m_colorImage || m_colorImage->width() != renderParams.outputWidth || m_colorImage->height() != renderParams.outputHeight) {
        m_colorImage = m_renderer->create_color_image(renderParams.outputWidth, renderParams.outputHeight, kColorFormat);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();

        m_outColor.update(m_colorImage.get());
    }

    if (!m_depthImage || m_depthImage->width() != renderParams.outputWidth || m_depthImage->height() != renderParams.outputHeight) {
        m_depthImage = m_renderer->create_depth_image(renderParams.outputWidth, renderParams.outputHeight);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();
    }

    if (!m_frameBuffer) {
        duk::rhi::Image* frameBufferAttachments[] = {m_colorImage.get(), m_depthImage.get()};

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

    m_objectEntries.clear();
    m_sortedObjectIndices.clear();
    m_paintEntries.clear();

    {
        std::set<Palette*> uniquePalettes;
        std::set<Painter*> uniquePainters;

        for (auto object : renderParams.scene->objects_with_components<MeshDrawing>()) {
            auto meshDrawing = object.component<MeshDrawing>();

            auto& objectEntry = m_objectEntries.emplace_back();
            objectEntry.objectId = object.id();
            objectEntry.brush = meshDrawing->mesh;
            objectEntry.painter = meshDrawing->painter;
            objectEntry.palette = meshDrawing->palette;
            objectEntry.sortKey = SortKey::calculate(meshDrawing);

            uniquePalettes.insert(objectEntry.palette);
            uniquePainters.insert(objectEntry.painter);
        }

        // resets all painters and palettes that are going to be used
        for (auto painter : uniquePainters) {
            painter->clear();
        }

        for (auto palette : uniquePalettes) {
            palette->clear();
        }
    }

    SortKey::sort_indices(m_objectEntries, m_sortedObjectIndices);

    auto compatible_with_paint_entry = [](const Renderer::PaintEntry& paintEntry, const Renderer::ObjectEntry& objectEntry) -> bool {
        return paintEntry.painter == objectEntry.painter && paintEntry.params.brush == objectEntry.brush && paintEntry.params.palette == objectEntry.palette;
    };

    auto is_valid = [](const Renderer::PaintEntry& paintEntry) {
        return paintEntry.painter && paintEntry.params.brush && paintEntry.params.palette && paintEntry.params.instanceCount > 0;
    };

    Renderer::PaintEntry paintEntry = {};
    paintEntry.params.renderPass = m_renderPass.get();
    paintEntry.params.outputWidth = renderParams.outputWidth;
    paintEntry.params.outputHeight = renderParams.outputHeight;

    for (auto sortedIndex : m_sortedObjectIndices) {
        auto& objectEntry = m_objectEntries[sortedIndex];

        // if this object belongs to another paint entry
        if (!compatible_with_paint_entry(paintEntry, objectEntry)) {

            // if this entry has an actual painter
            if (is_valid(paintEntry)) {
                m_paintEntries.push_back(paintEntry);
            }
            if (paintEntry.params.palette != objectEntry.palette || paintEntry.painter != objectEntry.painter) {
                paintEntry.params.instanceCount = 0;
                paintEntry.params.firstInstance = 0;
            }
            else if (paintEntry.params.brush != objectEntry.brush) {
                paintEntry.params.firstInstance += paintEntry.params.instanceCount;
                paintEntry.params.instanceCount = 0;
            }
            paintEntry.painter = objectEntry.painter;
            paintEntry.params.palette = objectEntry.palette;
            paintEntry.params.brush = objectEntry.brush;
        }

        Palette::InsertInstanceParams instanceParams = {};
        instanceParams.object = renderParams.scene->object(objectEntry.objectId);

        objectEntry.palette->insert_instance(instanceParams);

        paintEntry.params.instanceCount++;
    }
    if (is_valid(paintEntry)) {
        m_paintEntries.push_back(paintEntry);
    }

    renderParams.commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    // for each painter entry
    for (auto& [params, painter] : m_paintEntries) {
        painter->paint(renderParams.commandBuffer, params);
    }

    renderParams.commandBuffer->end_render_pass();
}

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

}