/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/painter.h>
#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/brushes/sprite_brush.h>

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

static void render_meshes(const Pass::RenderParams& renderParams, duk::rhi::RenderPass* renderPass, DrawData* drawData, MeshDrawData* meshDrawData) {

    drawData->clear();
    meshDrawData->clear();
    auto& objects = drawData->objects;
    auto& sortedObjects = drawData->sortedObjects;
    auto& drawEntries = meshDrawData->drawEntries;

    std::set<MeshMaterial*> uniqueMaterials;

    for (auto object : renderParams.scene->objects_with_components<MeshRenderer>()) {
        auto meshRenderer = object.component<MeshRenderer>();

        auto material = meshRenderer->material.get();

        auto& objectEntry = objects.emplace_back();
        objectEntry.objectId = object.id();
        objectEntry.brush = meshRenderer->mesh.get();
        objectEntry.material = material;
        objectEntry.sortKey = SortKey::calculate(*meshRenderer);

        uniqueMaterials.insert(material);
    }

    for (auto material : uniqueMaterials) {
        material->clear_instances();
    }

    SortKey::sort_indices(objects, sortedObjects);

    auto compatible_with_draw_entry = [](const MeshDrawEntry& drawEntry, const ObjectEntry& objectEntry) -> bool {
        return drawEntry.mesh == objectEntry.brush && drawEntry.material == objectEntry.material;
    };

    auto is_valid = [](const MeshDrawEntry& drawEntry) {
        return drawEntry.mesh && drawEntry.material && drawEntry.instanceCount > 0;
    };

    MeshDrawEntry drawEntry = {};
    drawEntry.params.renderPass = renderPass;
    drawEntry.params.outputWidth = renderParams.outputWidth;
    drawEntry.params.outputHeight = renderParams.outputHeight;
    drawEntry.params.globalDescriptors = renderParams.globalDescriptors;

    for (auto sortedIndex : sortedObjects) {
        auto& objectEntry = objects[sortedIndex];

        // if this object belongs to another draw entry
        if (!compatible_with_draw_entry(drawEntry, objectEntry)) {

            if (is_valid(drawEntry)) {
                drawEntries.push_back(drawEntry);
            }
            if (drawEntry.material != objectEntry.material) {
                drawEntry.instanceCount = 0;
                drawEntry.firstInstance = 0;
            }
            else if (drawEntry.mesh != objectEntry.brush) {
                drawEntry.firstInstance += drawEntry.instanceCount;
                drawEntry.instanceCount = 0;
            }
            drawEntry.material = reinterpret_cast<MeshMaterial*>(objectEntry.material);
            drawEntry.mesh = reinterpret_cast<Mesh*>(objectEntry.brush);
        }

        MeshMaterial::InsertInstanceParams instanceParams = {};
        instanceParams.object = renderParams.scene->object(objectEntry.objectId);

        drawEntry.material->insert_instance(instanceParams);

        drawEntry.instanceCount++;
    }
    if (is_valid(drawEntry)) {
        drawEntries.push_back(drawEntry);
    }

    // mark all instance buffers for gpu upload
    for (auto material : uniqueMaterials) {
        material->flush_instances();
    }

    // for each draw entry
    MeshMaterial* currentMaterial = nullptr;
    for (auto& entry : drawEntries) {
        if (currentMaterial != entry.material) {
            currentMaterial = entry.material;
            entry.material->apply(renderParams.commandBuffer, entry.params);
        }
        entry.mesh->draw(renderParams.commandBuffer, entry.instanceCount, entry.firstInstance);
    }
}

void render_sprites(const Pass::RenderParams& renderParams, duk::rhi::RenderPass* renderPass, DrawData* drawData, SpriteDrawData* spriteDrawData) {

    auto& spriteBrushPool = spriteDrawData->spriteBrushPool;

    for (auto object : renderParams.scene->objects_with_components<SpriteRenderer>()) {
        auto spriteRenderer = object.component<SpriteRenderer>();
        SpriteEntry spriteEntry = {};
        spriteEntry.sprite = spriteRenderer->sprite.get();
        spriteEntry.object = object;

//        spriteBrush.push()
    }

}

}

void DrawData::clear() {
    objects.clear();
    sortedObjects.clear();
}

void MeshDrawData::clear() {
    drawEntries.clear();
}

ForwardPass::ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo) :
    m_renderer(forwardPassCreateInfo.renderer),
    m_outColor(duk::rhi::Access::COLOR_ATTACHMENT_WRITE, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT, duk::rhi::Image::Layout::SHADER_READ_ONLY) {

    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = detail::kColorFormat;
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
        m_colorImage = m_renderer->create_color_image(renderParams.outputWidth, renderParams.outputHeight, detail::kColorFormat);

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

    renderParams.commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    detail::render_meshes(renderParams, m_renderPass.get(), &m_drawData, &m_meshDrawData);

//    detail::render_sprites(renderParams, m_renderPass.get());

    renderParams.commandBuffer->end_render_pass();
}

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

}