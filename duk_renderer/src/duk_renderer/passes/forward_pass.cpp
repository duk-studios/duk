/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/mesh_drawing.h>
#include <duk_renderer/materials/material.h>
#include <duk_renderer/materials/painter.h>
#include <duk_renderer/brushes/mesh.h>

namespace duk::renderer {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

ForwardPass::ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo) :
    m_renderer(forwardPassCreateInfo.renderer),
    m_outColor(duk::rhi::Access::COLOR_ATTACHMENT_WRITE, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT, duk::rhi::Image::Layout::SHADER_READ_ONLY) {

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

    std::set<Material*> uniqueMaterials;

    for (auto object : renderParams.scene->objects_with_components<MeshDrawing>()) {
        auto meshDrawing = object.component<MeshDrawing>();

        auto& objectEntry = m_objectEntries.emplace_back();
        objectEntry.objectId = object.id();
        objectEntry.brush = meshDrawing->mesh.get();
        objectEntry.material = meshDrawing->material.get();
        objectEntry.sortKey = SortKey::calculate(*meshDrawing);

        uniqueMaterials.insert(objectEntry.material);
    }

    for (auto material : uniqueMaterials) {
        material->clear_instances();
    }

    SortKey::sort_indices(m_objectEntries, m_sortedObjectIndices);

    auto compatible_with_paint_entry = [](const PaintEntry& paintEntry, const ObjectEntry& objectEntry) -> bool {
        return paintEntry.params.brush == objectEntry.brush && paintEntry.material == objectEntry.material;
    };

    auto is_valid = [](const PaintEntry& paintEntry) {
        return paintEntry.params.brush && paintEntry.material && paintEntry.params.instanceCount > 0;
    };

    PaintEntry paintEntry = {};
    paintEntry.params.renderPass = m_renderPass.get();
    paintEntry.params.outputWidth = renderParams.outputWidth;
    paintEntry.params.outputHeight = renderParams.outputHeight;
    paintEntry.params.globalDescriptors = renderParams.globalDescriptors;

    for (auto sortedIndex : m_sortedObjectIndices) {
        auto& objectEntry = m_objectEntries[sortedIndex];

        // if this object belongs to another paint entry
        if (!compatible_with_paint_entry(paintEntry, objectEntry)) {

            // if this entry has an actual painter
            if (is_valid(paintEntry)) {
                m_paintEntries.push_back(paintEntry);
            }
            if (paintEntry.material != objectEntry.material) {
                paintEntry.params.instanceCount = 0;
                paintEntry.params.firstInstance = 0;
            }
            else if (paintEntry.params.brush != objectEntry.brush) {
                paintEntry.params.firstInstance += paintEntry.params.instanceCount;
                paintEntry.params.instanceCount = 0;
            }
            paintEntry.material = objectEntry.material;
            paintEntry.params.brush = objectEntry.brush;
        }

        Material::InsertInstanceParams instanceParams = {};
        instanceParams.object = renderParams.scene->object(objectEntry.objectId);

        objectEntry.material->insert_instance(instanceParams);

        paintEntry.params.instanceCount++;
    }
    if (is_valid(paintEntry)) {
        m_paintEntries.push_back(paintEntry);
    }

    // mark all instance buffers for gpu upload
    for (auto material : uniqueMaterials) {
        material->flush_instances();
    }

    renderParams.commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    // for each paint entry
    for (auto& entry : m_paintEntries) {
        entry.material->paint(renderParams.commandBuffer, entry.params);
    }

    renderParams.commandBuffer->end_render_pass();
}

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

}